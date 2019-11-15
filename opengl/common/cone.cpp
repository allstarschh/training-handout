/*************************************************************************
 *
 * Copyright (c) 2016, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include "cone.h"

#include <QOpenGLShaderProgram>

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>

const float pi = static_cast<float>(M_PI);
const float twoPi = 2.0 * pi;

Cone::Cone( QObject* parent )
    : QObject( parent ),
      m_hasTopEndcap( true ),
      m_hasBottomEndcap( true ),
      m_topRadius( 0.0f ),
      m_bottomRadius( 1.0f ),
      m_height( 2.0f ),
      m_rings( 16 ),
      m_slices( 16 ),
      m_positionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_normalBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_vao()
{
}

void Cone::setMaterial( const MaterialPtr& material )
{
    if ( material == m_material )
        return;
    m_material = material;
    updateVertexArrayObject();
}

MaterialPtr Cone::material() const
{
    return m_material;
}

void Cone::create()
{
    // Allocate some storage to hold per-vertex data
    QVector<QVector3D> v;     // Vertices
    QVector<QVector3D> n;     // Normals
    QVector<unsigned int> el; // Element indices

    // Generate the vertex data
    generateVertexData( &v, &n, &el );

    // Create a vertex array object and bind it so we don't clobber
    // any currently bound VAO when we bind the index buffer below
    if ( !m_vao.isCreated() )
        m_vao.create();
    m_vao.bind();

    // Create and populate the buffer objects
    m_positionBuffer.create();
    m_positionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_positionBuffer.bind();
    m_positionBuffer.allocate( v.constData(), v.size() * sizeof( QVector3D ) );

    m_normalBuffer.create();
    m_normalBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_normalBuffer.bind();
    m_normalBuffer.allocate( n.constData(), n.size() * sizeof( QVector3D ) );

    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.bind();
    m_indexBuffer.allocate( el.constData(), el.size() * sizeof( unsigned int ) );

    m_vao.release();

    updateVertexArrayObject();
}

void Cone::generateVertexData( QVector<QVector3D>* vertices,
                               QVector<QVector3D>* normals,
                               QVector<unsigned int>* indices )
{
    int faces = slices() * rings();           // Number of "rectangular" faces
    int nVerts  = ( slices() + 1 ) * rings()     // sides
            + (( hasTopEndcap() + hasBottomEndcap() ) * ( slices() + 1 )); // endcaps

    // Resize vector to hold our data
    vertices->resize( nVerts );
    normals->resize( nVerts );
    indices->resize( 6 * faces + ( hasTopEndcap() + hasBottomEndcap() ) * 3 * slices() );

    // The side of the cone
    const float dTheta = twoPi / static_cast<float>( m_slices );
    const float dy = m_height / static_cast<float>( m_rings - 1 );

    // Iterate over heights (rings)
    int index = 0;

    for ( int ring = 0; ring < m_rings; ++ring )
    {
        const float y = -m_height / 2.0f + static_cast<float>( ring ) * dy;
        const float t = (y + m_height / 2) / m_height;
        const float radius = (m_bottomRadius * (1 - t)) + (t * m_topRadius);

        // Iterate over slices (segments in a ring)
        for ( int slice = 0; slice < m_slices + 1; ++slice )
        {
            const float theta = static_cast<float>( slice ) * dTheta;
            const float tanAlpha = tanf((pi/2) - atan(m_height / (m_bottomRadius - m_topRadius)));
            const float cosTheta = cosf( theta );
            const float sinTheta = sinf( theta );
            // Position
            QVector3D &position = (*vertices)[index];
            position.setX( radius * cosTheta );
            position.setY( y );
            position.setZ( radius * sinTheta );

            // Normal
            QVector3D &normal = (*normals)[index];
            normal.setX( cosTheta );
            normal.setY( tanAlpha );
            normal.setZ( sinTheta );

            normal.normalize();

            ++index;
        }
    }

    int elIndex = 0;
    for ( int i = 0; i < rings() - 1; ++i )
    {
        const int ringStartIndex = i * ( slices() + 1 );
        const int nextRingStartIndex = ( i + 1 ) * ( slices() + 1 );

        for ( int j = 0; j < slices(); ++j )
        {
            // Split the quad into two triangles
            (*indices)[elIndex]   = ringStartIndex + j;
            (*indices)[elIndex+1] = nextRingStartIndex + j;
            (*indices)[elIndex+2] = ringStartIndex + j + 1;
            (*indices)[elIndex+3] = ringStartIndex + j + 1;
            (*indices)[elIndex+4] = nextRingStartIndex + j;
            (*indices)[elIndex+5] = nextRingStartIndex + j + 1;

            elIndex += 6;
        }
    }

    // Generate the vertex data and indices for the two end caps
    if ( m_hasTopEndcap ) {
        generateEndCapVertexData( m_height / 2.0f, &index, &elIndex,
                              vertices, normals, indices, true );
    }
    if ( m_hasBottomEndcap ) {
        generateEndCapVertexData( -m_height / 2.0f, &index, &elIndex,
                              vertices, normals, indices, false );
    }
}

void Cone::generateEndCapVertexData( float y,
                                     int* index,
                                     int* elIndex,
                                     QVector<QVector3D>* vertices,
                                     QVector<QVector3D>* normals,
                                     QVector<unsigned int>* indices,
                                     bool isTopCap )
{
    // Make a note of the vertex index for the center of the end cap
    int endCapStartIndex = *index;

    (*vertices)[*index].setX( 0.0f );
    (*vertices)[*index].setY( y );
    (*vertices)[*index].setZ( 0.0f );

    (*normals)[*index].setX( 0.0f );
    (*normals)[*index].setY( ( y >= 0.0f ) ? 1.0f : -1.0 );
    (*normals)[*index].setZ( 0.0f );

    ++*index;

    const float dTheta = twoPi / static_cast<float>( m_slices );
    for ( int slice = 0; slice < m_slices; ++slice )
    {
        const float theta = static_cast<float>( slice ) * dTheta;
        const float cosTheta = cosf( theta );
        const float sinTheta = sinf( theta );

        const float t = (y + m_height / 2) / m_height;
        const float radius = (m_bottomRadius * (1 - t)) + (t * m_topRadius);

        QVector3D &position = (*vertices)[*index];
        position.setX( radius * cosTheta );
        position.setY( y );
        position.setZ( radius * sinTheta );

        QVector3D &normal = (*normals)[*index];
        normal.setX( 0.0f );
        normal.setY( ( y >= 0.0f ) ? 1.0f : -1.0 );
        normal.setZ( 0.0f );

        ++*index;
    }

    if ( isTopCap ) {
        for ( int i = m_slices - 1 ; i >= 0 ; --i )
        {
            if ( i != 0 ) {
                (*indices)[*elIndex]   = endCapStartIndex;
                (*indices)[*elIndex+1] = endCapStartIndex + i + 1;
                (*indices)[*elIndex+2] = endCapStartIndex + i;
            } else {
                (*indices)[*elIndex]   = endCapStartIndex;
                (*indices)[*elIndex+1] = endCapStartIndex + i + 1;
                (*indices)[*elIndex+2] = endCapStartIndex + m_slices;
            }

            *elIndex += 3;
        }
    } else {
        for ( int i = 0 ; i < m_slices; ++i )
        {
            if ( i != m_slices - 1 ) {
                (*indices)[*elIndex]   = endCapStartIndex;
                (*indices)[*elIndex+1] = endCapStartIndex + i + 1;
                (*indices)[*elIndex+2] = endCapStartIndex + i + 2;
            } else {
                (*indices)[*elIndex]   = endCapStartIndex;
                (*indices)[*elIndex+1] = endCapStartIndex + i + 1;
                (*indices)[*elIndex+2] = endCapStartIndex + 1;
            }

            *elIndex += 3;
        }
    }
}

int Cone::indexCount() const
{
    return 6 * slices() * rings()   // sides
            + ( ( hasTopEndcap() + hasBottomEndcap() ) * 3 * slices() ); // endcaps
}


void Cone::updateVertexArrayObject()
{
    // Ensure that we have a valid material and geometry
    if ( !m_material || !m_positionBuffer.isCreated() )
        return;

    m_vao.bind();
    setupVertexArrays();

    // End VAO setup
    m_vao.release();
}

void Cone::setupVertexArrays()
{
    QOpenGLShaderProgramPtr shader = m_material->shader();
    shader->bind();

    m_positionBuffer.bind();
    shader->enableAttributeArray( "vertexPosition" );
    shader->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );

    m_normalBuffer.bind();
    shader->enableAttributeArray( "vertexNormal" );
    shader->setAttributeBuffer( "vertexNormal", GL_FLOAT, 0, 3 );
    m_normalBuffer.release();

    m_indexBuffer.bind();
}

void Cone::render()
{
    // Bind the vertex array oobject to set up our vertex buffers and index buffer
    m_vao.bind();

    QOpenGLFunctions* m_funcs = QOpenGLContext::currentContext()->functions();
    // Draw it!
    m_funcs->glDrawElements( GL_TRIANGLES, indexCount(), GL_UNSIGNED_INT, nullptr );

    m_vao.release();
}

void Cone::setHasTopEndcap ( bool arg )
{
    if (arg != hasTopEndcap()) {
        m_hasTopEndcap = arg;
        emit hasTopEndcapChanged(arg);
    }
}

void Cone::setHasBottomEndcap ( bool arg )
{
    if (arg != hasBottomEndcap()) {
        m_hasBottomEndcap = arg;
        emit hasBottomEndcapChanged(arg);
    }
}

void Cone::setTopRadius( float arg )
{
    if (arg != topRadius()) {
        if (arg >= 0.0) {
            m_topRadius = arg;
            emit topRadiusChanged(arg);
        } else {
            qWarning() << "Radius must be 0.0 or greater";
        }
    }
}

void Cone::setBottomRadius( float arg )
{
    if (arg != bottomRadius()) {
        if (arg >= 0.0) {
            m_bottomRadius = arg;
            emit bottomRadiusChanged(arg);
        } else {
            qWarning() << "Radius must be 0.0 or greater";
        }
    }
}

void Cone::setHeight( float arg )
{
    if (arg != height()) {
        if (arg > 0.0) {
            m_height = arg;
            emit heightChanged(arg);
        } else {
            qWarning() << "Height must be greater than 0.0";
        }
    }
}

void Cone::setRings( int arg )
{
    if (arg != rings()) {
        if (arg >= 2) {
            m_rings = arg;
            emit ringsChanged(arg);
        } else {
            qWarning() << "Rings must be 2 or greater";
        }
    }
}

void Cone::setSlices( int arg )
{
    if (arg != slices()) {
        if (arg >= 3) {
            m_slices = arg;
            emit slicesChanged(arg);
        } else {
            qWarning() << "Slices must be 3 or greater";
        }
    }
}
