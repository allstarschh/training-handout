#include "objloader.h"

#include "mesh.h"
#include "axisalignedboundingbox.h"

#include <QDebug>
#include <QFile>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QTextStream>
#include <QVector>

inline uint qHash(const FaceIndices &faceIndices)
{
    return faceIndices.positionIndex
            + 10 * faceIndices.texCoordIndex
            + 100 * faceIndices.normalIndex;
}

ObjLoader::ObjLoader()
    : m_loadTextureCoords( true ),
      m_generateTangents( true ),
      m_centerMesh( false )
{
}

bool ObjLoader::load( const QString& fileName )
{
    QFile file( fileName );
    if ( !file.open( ::QIODevice::ReadOnly | ::QIODevice::Text ) )
    {
        qDebug() << "Could not open file" << fileName << "for reading";
        return false;
    }

    return load( &file );
}

static void addFaceVertex( const FaceIndices& faceIndices,
                           QVector<FaceIndices>& faceIndexVector,
                           QHash<FaceIndices, unsigned int>& faceIndexMap )
{
    if (faceIndices.positionIndex != std::numeric_limits<unsigned int>::max()) {
        faceIndexVector.append(faceIndices);
        if (!faceIndexMap.contains(faceIndices))
            faceIndexMap.insert(faceIndices, faceIndexMap.size());
    } else {
        qWarning( "Missing position index" );
    }
}

bool ObjLoader::load( QIODevice* ioDev )
{
    Q_CHECK_PTR(ioDev);
    if (!ioDev->isOpen()) {
        qWarning() << "iodevice" << ioDev << "not open for reading";
        return false;
    }

    int faceCount = 0;

    // Parse faces taking into account each vertex in a face can index different indices
    // for the positions, normals and texture coords;
    // Generate unique vertices (in OpenGL parlance) and output to m_points, m_texCoords,
    // m_normals and calculate mapping from faces to unique indices
    QVector<QVector3D> positions;
    QVector<QVector3D> normals;
    QVector<QVector2D> texCoords;
    QHash<FaceIndices, unsigned int> faceIndexMap;
    QVector<FaceIndices> faceIndexVector;

    QTextStream stream(ioDev);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        line = line.simplified();

        if (line.length() > 0 && line.at(0) != QChar::fromLatin1('#')) {
            QTextStream lineStream(&line, QIODevice::ReadOnly);
            QString token;
            lineStream >> token;

            if (token == QStringLiteral("v")) {
                float x, y, z;
                lineStream >> x >> y >> z;
                positions.append(QVector3D( x, y, z ));
            } else if (token == QStringLiteral("vt") && m_loadTextureCoords) {
                // Process texture coordinate
                float s,t;
                lineStream >> s >> t;
                texCoords.append(QVector2D(s, t));
            } else if (token == QStringLiteral("vn")) {
                float x, y, z;
                lineStream >> x >> y >> z;
                normals.append(QVector3D( x, y, z ));
            } else if (token == QStringLiteral("f")) {
                // Process face
                ++faceCount;
                QVector<FaceIndices> face;
                int faceVertices = 0;
                while (!lineStream.atEnd()) {
                    QString faceString;
                    lineStream >> faceString;

                    FaceIndices faceIndices;
                    QStringList indices = faceString.split(QChar::fromLatin1('/'));
                    switch (indices.size()) {
                    case 3:
                        faceIndices.normalIndex = indices.at(2).toInt() - 1;  // fall through
                    case 2:
                        faceIndices.texCoordIndex = indices.at(1).toInt() - 1; // fall through
                    case 1:
                        faceIndices.positionIndex = indices.at(0).toInt() - 1;
                        break;
                    default:
                        qWarning() << "Unsupported number of indices in face element";
                    }

                    face.append(faceIndices);
                    ++faceVertices;
                }

                // If number of edges in face is greater than 3,
                // decompose into triangles as a triangle fan.
                FaceIndices v0 = face[0];
                FaceIndices v1 = face[1];
                FaceIndices v2 = face[2];

                // First face
                addFaceVertex(v0, faceIndexVector, faceIndexMap);
                addFaceVertex(v1, faceIndexVector, faceIndexMap);
                addFaceVertex(v2, faceIndexVector, faceIndexMap);

                for ( int i = 3; i < face.size(); ++i ) {
                    v1 = v2;
                    v2 = face[i];
                    addFaceVertex(v0, faceIndexVector, faceIndexMap);
                    addFaceVertex(v1, faceIndexVector, faceIndexMap);
                    addFaceVertex(v2, faceIndexVector, faceIndexMap);
                }
            } // end of face
        } // end of input line
    } // while (!stream.atEnd())

    updateIndices(positions, normals, texCoords, faceIndexMap, faceIndexVector);

    if (m_normals.isEmpty())
        generateAveragedNormals(m_points, m_normals, m_indices);

    if (m_generateTangents && !m_texCoords.isEmpty())
        generateTangents(m_points, m_normals, m_indices, m_texCoords, m_tangents);

    if (m_centerMesh)
        center(m_points);

    qDebug() << "Loaded mesh:";
    qDebug() << " " << m_points.size() << "points";
    qDebug() << " " << faceCount << "faces";
    qDebug() << " " << m_indices.size() / 3 << "triangles.";
    qDebug() << " " << m_normals.size() << "normals";
    qDebug() << " " << m_tangents.size() << "tangents ";
    qDebug() << " " << m_texCoords.size() << "texture coordinates.";

    return true;
}

void ObjLoader::updateIndices( const QVector<QVector3D>& positions,
                               const QVector<QVector3D>& normals,
                               const QVector<QVector2D>& texCoords,
                               const QHash<FaceIndices, unsigned int>& faceIndexMap,
                               const QVector<FaceIndices>& faceIndexVector )
{
    // Iterate over the faceIndexMap and pull out pos, texCoord and normal data
    // thereby generating unique vertices of data (by OpenGL definition)
    const int vertexCount = faceIndexMap.size();
    const bool hasTexCoords = !texCoords.isEmpty();
    const bool hasNormals = !normals.isEmpty();

    m_points.resize(vertexCount);
    m_texCoords.clear();
    if (hasTexCoords)
        m_texCoords.resize(vertexCount);
    m_normals.clear();
    if (hasNormals)
        m_normals.resize(vertexCount);

    for (auto it = faceIndexMap.constBegin(); it != faceIndexMap.constEnd(); ++it)
    {
        const FaceIndices &faceIndices = it.key();
        const int i = it.value();

        m_points[i] = positions[faceIndices.positionIndex];
        if (hasTexCoords)
            m_texCoords[i] = texCoords[faceIndices.texCoordIndex];
        if (hasNormals)
            m_normals[i] = normals[faceIndices.normalIndex];
    }

    // Now iterate over the face indices and lookup the unique vertex index
    const int indexCount = faceIndexVector.size();
    m_indices.clear();
    m_indices.reserve(indexCount);
    foreach (const FaceIndices &faceIndices, faceIndexVector) {
        const unsigned int i = faceIndexMap.value(faceIndices);
        m_indices.append(i);
    }
}

void ObjLoader::generateAveragedNormals( const QVector<QVector3D>& points,
                                         QVector<QVector3D>& normals,
                                         const QVector<unsigned int>& faces ) const
{
    for ( int i = 0; i < points.size(); ++i )
        normals.append( QVector3D() );

    for ( int i = 0; i < faces.size(); i += 3 )
    {
        const QVector3D& p1 = points[ faces[i]   ];
        const QVector3D& p2 = points[ faces[i+1] ];
        const QVector3D& p3 = points[ faces[i+2] ];

        QVector3D a = p2 - p1;
        QVector3D b = p3 - p1;
        QVector3D n = QVector3D::crossProduct( a, b ).normalized();

        normals[ faces[i]   ] += n;
        normals[ faces[i+1] ] += n;
        normals[ faces[i+2] ] += n;
    }

    for ( int i = 0; i < normals.size(); ++i )
        normals[i].normalize();
}

void ObjLoader::generateTangents( const QVector<QVector3D>& points,
                                  const QVector<QVector3D>& normals,
                                  const QVector<unsigned  int>& faces,
                                  const QVector<QVector2D>& texCoords,
                                  QVector<QVector4D>& tangents ) const
{
    tangents.clear();
    QVector<QVector3D> tan1Accum;
    QVector<QVector3D> tan2Accum;

    for ( int i = 0; i < points.size(); i++ )
    {
        tan1Accum.append( QVector3D() );
        tan2Accum.append( QVector3D() );
        tangents.append( QVector4D() );
    }

    // Compute the tangent vector
    for ( int i = 0; i < faces.size(); i += 3 )
    {
        const QVector3D& p1 = points[ faces[i] ];
        const QVector3D& p2 = points[ faces[i+1] ];
        const QVector3D& p3 = points[ faces[i+2] ];

        const QVector2D& tc1 = texCoords[ faces[i] ];
        const QVector2D& tc2 = texCoords[ faces[i+1] ];
        const QVector2D& tc3 = texCoords[ faces[i+2] ];

        QVector3D q1 = p2 - p1;
        QVector3D q2 = p3 - p1;
        float s1 = tc2.x() - tc1.x(), s2 = tc3.x() - tc1.x();
        float t1 = tc2.y() - tc1.y(), t2 = tc3.y() - tc1.y();
        float r = 1.0f / ( s1 * t2 - s2 * t1 );
        QVector3D tan1( ( t2 * q1.x() - t1 * q2.x() ) * r,
                        ( t2 * q1.y() - t1 * q2.y() ) * r,
                        ( t2 * q1.z() - t1 * q2.z() ) * r );
        QVector3D tan2( ( s1 * q2.x() - s2 * q1.x() ) * r,
                        ( s1 * q2.y() - s2 * q1.y() ) * r,
                        ( s1 * q2.z() - s2 * q1.z() ) * r );
        tan1Accum[ faces[i]   ] += tan1;
        tan1Accum[ faces[i+1] ] += tan1;
        tan1Accum[ faces[i+2] ] += tan1;
        tan2Accum[ faces[i]   ] += tan2;
        tan2Accum[ faces[i+1] ] += tan2;
        tan2Accum[ faces[i+2] ] += tan2;
    }

    for ( int i = 0; i < points.size(); ++i )
    {
        const QVector3D& n = normals[i];
        QVector3D& t1 = tan1Accum[i];
        QVector3D& t2 = tan2Accum[i];

        // Gram-Schmidt orthogonalize
        tangents[i] = QVector4D( QVector3D( t1 - QVector3D::dotProduct( n, t1 ) * n ).normalized(), 0.0f );

        // Store handedness in w
        tangents[i].setW( ( QVector3D::dotProduct( QVector3D::crossProduct( n, t1 ), t2 ) < 0.0f ) ? -1.0f : 1.0f );
    }
}

void ObjLoader::center( QVector<QVector3D>& points )
{
    if ( points.isEmpty() )
        return;

    AxisAlignedBoundingBox bb(points);
    QVector3D center = bb.center();

    // Translate center of the AABB to the origin
    for ( int i = 0; i < points.size(); ++i )
    {
        QVector3D& point = points[i];
        point = point - center;
    }
}
