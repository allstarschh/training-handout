#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

#include <limits>

class QString;
class QIODevice;

struct FaceIndices
{
    FaceIndices()
        : positionIndex(std::numeric_limits<unsigned int>::max())
        , texCoordIndex(std::numeric_limits<unsigned int>::max())
        , normalIndex(std::numeric_limits<unsigned int>::max())
    {}

    FaceIndices(unsigned int posIndex, unsigned int tcIndex, unsigned int nIndex)
        : positionIndex(posIndex)
        , texCoordIndex(tcIndex)
        , normalIndex(nIndex)
    {}

    bool operator == (const FaceIndices &other) const
    {
        return positionIndex == other.positionIndex &&
               texCoordIndex == other.texCoordIndex &&
               normalIndex == other.normalIndex;
    }

    unsigned int positionIndex;
    unsigned int texCoordIndex;
    unsigned int normalIndex;
};

class ObjLoader
{
public:
    ObjLoader();

    void setLoadTextureCoordinatesEnabled( bool b ) { m_loadTextureCoords = b; }
    bool isLoadTextureCoordinatesEnabled() const { return m_loadTextureCoords; }

    void setTangentGenerationEnabled( bool b ) { m_generateTangents = b; }
    bool isTangentGenerationEnabled() const { return m_generateTangents; }

    void setMeshCenteringEnabled( bool b ) { m_centerMesh = b; }
    bool isMeshCenteringEnabled() const { return m_centerMesh; }

    bool hasNormals() const { return !m_normals.isEmpty(); }
    bool hasTextureCoordinates() const { return !m_texCoords.isEmpty(); }
    bool hasTangents() const { return !m_tangents.isEmpty(); }

    bool load( const QString& fileName );
    bool load( QIODevice* ioDev );

    QVector<QVector3D> vertices() const { return m_points; }
    QVector<QVector3D> normals() const { return m_normals; }
    QVector<QVector2D> textureCoordinates() const { return m_texCoords; }
    QVector<QVector4D> tangents() const { return m_tangents; }
    QVector<unsigned int> indices() const { return m_indices; }

private:
    void updateIndices(const QVector<QVector3D> &positions,
                       const QVector<QVector3D> &normals,
                       const QVector<QVector2D> &texCoords,
                       const QHash<FaceIndices, unsigned int> &faceIndexMap,
                       const QVector<FaceIndices> &faceIndexVector);
    void generateAveragedNormals( const QVector<QVector3D>& points,
                                  QVector<QVector3D>& normals,
                                  const QVector<unsigned int>& faces ) const;
    void generateTangents( const QVector<QVector3D>& points,
                           const QVector<QVector3D>& normals,
                           const QVector<unsigned int>& faces,
                           const QVector<QVector2D>& texCoords,
                           QVector<QVector4D>& tangents ) const;
    void center( QVector<QVector3D>& points );

    bool m_loadTextureCoords;
    bool m_generateTangents;
    bool m_centerMesh;

    QVector<QVector3D> m_points;
    QVector<QVector3D> m_normals;
    QVector<QVector2D> m_texCoords;
    QVector<QVector4D> m_tangents;
    QVector<unsigned int> m_indices;
};

#endif // OBJLOADER_H
