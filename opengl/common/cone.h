/*************************************************************************
 *
 * Copyright (c) 2016, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#ifndef CONE_H
#define CONE_H

#include <QObject>

#include "material.h"
#include "qopenglvertexarrayobject.h"

#include <QOpenGLBuffer>

class Cone : public QObject
{
    Q_OBJECT

    Q_PROPERTY( bool hasTopEndcap READ hasTopEndcap WRITE setHasTopEndcap NOTIFY hasTopEndcapChanged )
    Q_PROPERTY( bool hasBottomEndcap READ hasBottomEndcap WRITE setHasBottomEndcap NOTIFY hasBottomEndcapChanged )
    Q_PROPERTY( float topRadius READ topRadius WRITE setTopRadius NOTIFY topRadiusChanged )
    Q_PROPERTY( float bottomRadius READ bottomRadius WRITE setBottomRadius NOTIFY bottomRadiusChanged )
    Q_PROPERTY( float height READ height WRITE setHeight NOTIFY heightChanged )
    Q_PROPERTY( int rings READ rings WRITE setRings NOTIFY ringsChanged )
    Q_PROPERTY( int slices READ slices WRITE setSlices NOTIFY slicesChanged )

public:
    explicit Cone( QObject* parent = nullptr );

    void setMaterial( const MaterialPtr& material );
    MaterialPtr material() const;

    bool hasTopEndcap() const      { return m_hasTopEndcap; }
    bool hasBottomEndcap() const   { return m_hasBottomEndcap; }
    float topRadius() const         { return m_topRadius; }
    float bottomRadius() const      { return m_bottomRadius; }
    float height() const            { return m_height; }
    int rings() const               { return m_rings; }
    int slices() const              { return m_slices; }

    QOpenGLVertexArrayObject* vertexArrayObject() { return &m_vao; }

    int indexCount() const;
    void updateVertexArrayObject();
    void setupVertexArrays();

public slots:
    void setHasTopEndcap ( bool arg );
    void setHasBottomEndcap ( bool arg );
    void setTopRadius( float arg );
    void setBottomRadius( float arg );
    void setHeight( float arg );
    void setRings( int arg );
    void setSlices( int arg );

    void create();
    void render();

Q_SIGNALS:
    void hasTopEndcapChanged( bool hasTopEndcap );
    void hasBottomEndcapChanged( bool hasBottomEndcap );
    void topRadiusChanged( float topRadius );
    void bottomRadiusChanged( float bottomRadius );
    void heightChanged( float height );
    void ringsChanged( int rings );
    void slicesChanged( int slices );

private:
    void generateVertexData( QVector<QVector3D>* vertices,
                             QVector<QVector3D>* normals,
                             QVector<unsigned int>* indices );
    void generateEndCapVertexData( float y,
                                   int* vertexIndex,
                                   int* elIndex,
                                   QVector<QVector3D>* vertices,
                                   QVector<QVector3D>* normals,
                                   QVector<unsigned int>* indices,
                                   bool isTopCap );

    MaterialPtr m_material;

    bool    m_hasTopEndcap;
    bool    m_hasBottomEndcap;
    float   m_topRadius;
    float   m_bottomRadius;
    float   m_height;
    int     m_rings;  // Rings of latitude
    int     m_slices; // Longitude

    // Buffers to hold the data
    QOpenGLBuffer m_positionBuffer;
    QOpenGLBuffer m_normalBuffer;
    QOpenGLBuffer m_indexBuffer;

    QOpenGLVertexArrayObject m_vao;
};

#endif // CONE_H
