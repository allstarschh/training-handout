#include "axisgnomon.h"

#include "camera.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>

static const float s_cylinderRadiusPercentage = 0.03f;
static const float s_coneLengthPercentage = 0.40f;
static const float s_coneRadiusPercentage = 0.06f;


AxisGnomon::AxisGnomon(QObject *parent)
    : QObject(parent)
    , m_cylinderSides(8)
    , m_size(40)
    , m_windowSize(800, 600)
    , m_position(40, 40)
    , m_material(new Material)
    , m_positionBuffer(QOpenGLBuffer::VertexBuffer)
    , m_colorBuffer(QOpenGLBuffer::VertexBuffer)
    , m_indexBuffer(QOpenGLBuffer::IndexBuffer)
{
    create();
}

AxisGnomon::AxisGnomon(int cylinderSides, QObject *parent)
    : QObject(parent)
    , m_cylinderSides(qMax(cylinderSides, 3))
    , m_size(40)
    , m_windowSize(800, 600)
    , m_position(40, 40)
    , m_material(new Material)
    , m_positionBuffer(QOpenGLBuffer::VertexBuffer)
    , m_colorBuffer(QOpenGLBuffer::VertexBuffer)
    , m_indexBuffer(QOpenGLBuffer::IndexBuffer)
{
    create();
}

int AxisGnomon::size() const
{
    return m_size;
}

void AxisGnomon::setSize(int size)
{
    if (m_size == size)
        return;

    m_size = size;
    updateProjectionMatrix();
}

QSize AxisGnomon::windowSize() const
{
    return m_windowSize;
}

void AxisGnomon::setWindowSize(QSize size)
{
    if (m_windowSize == size)
        return;

    m_windowSize = size;
    updateProjectionMatrix();
}

QPoint AxisGnomon::position() const
{
    return m_position;
}

void AxisGnomon::setPosition(QPoint position)
{
    if (m_position == position)
        return;

    m_position = position;
    updateProjectionMatrix();
}


int AxisGnomon::indexCount() const
{
    return ((   (2 * m_cylinderSides)  // cylinder
             + (2 * m_cylinderSides)  // cone
             + (2 * m_cylinderSides)  // cone cap
            ) * 3 + 12 ) // * axisCount + base cube
            * 3; // vertsPerTriangle
}

void AxisGnomon::create()
{
    if (!m_material)
        return;

    updateProjectionMatrix();

    QVector<float> v;
    QVector<float> col;
    QVector<unsigned int> el;

    m_material->setShaders( ":/shaders/axisgnomon.vert",
                            ":/shaders/axisgnomon.frag" );

    generateVertexData(v, col, el);

    m_vao.create();
    m_vao.bind();

    m_positionBuffer.create();
    m_positionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_positionBuffer.bind();
    m_positionBuffer.allocate( v.constData(), v.size() * sizeof( float ) );

    m_colorBuffer.create();
    m_colorBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_colorBuffer.bind();
    m_colorBuffer.allocate( col.constData(), col.size() * sizeof( float ) );

    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.bind();
    m_indexBuffer.allocate( el.constData(), el.size() * sizeof( unsigned int ) );

    QOpenGLShaderProgramPtr shader = m_material->shader();
    shader->bind();

    m_positionBuffer.bind();
    shader->enableAttributeArray( "vertexPosition" );
    shader->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );

    m_colorBuffer.bind();
    shader->enableAttributeArray( "vertexColor" );
    shader->setAttributeBuffer( "vertexColor", GL_FLOAT, 0, 3 );

    m_indexBuffer.bind();

    m_vao.release();
}

void AxisGnomon::render(Camera *camera)
{
    QOpenGLFunctions *functions = QOpenGLContext::currentContext()->functions();
    functions->glClear( GL_DEPTH_BUFFER_BIT );

    m_material->bind();

    const QMatrix4x4 viewRotation(camera->viewMatrix().normalMatrix());
    const QMatrix4x4 mvp = m_projection * viewRotation;
    m_material->shader()->setUniformValue( "mvp", mvp );

    m_vao.bind();
    functions->glDrawElements( GL_TRIANGLES, indexCount(), GL_UNSIGNED_INT, nullptr );
    m_vao.release();
}

void AxisGnomon::generateVertexData(QVector<float> &vertices, QVector<float> &color, QVector<unsigned int> &indices)
{
    const int sides = m_cylinderSides;
    const int axisVerts = 2 * sides + 2 * sides + 2; // cylinder + cone-sides + cone-top/bottom
    const int axisFloats = 3 * axisVerts;
    const int axisElements = 6 * sides;
    const int cubeFloats = 8 * 3;
    const float cylinderRadius = s_cylinderRadiusPercentage;
    const float cylinderStart = cylinderRadius;
    const float cylinderEnd = (1.0f - s_coneLengthPercentage);

    vertices.resize(axisFloats * 3 + cubeFloats);
    color.resize(vertices.size());
    indices.resize((axisElements * 3 + 12) * 3);
    Q_ASSERT(indices.size() == indexCount());

    const int xAxisVertsOffset = 0;
    const int yAxisVertsOffset = axisVerts;
    const int zAxisVertsOffset = 2*axisVerts;

    // cylinder verts:
    for (int i = 0; i < sides; ++i) {
        const float angle = (i * 2 * M_PI) / sides;
        const float coord1 = sin(angle) * cylinderRadius;
        const float coord2 = cos(angle) * cylinderRadius;

        vertices[3*(i + xAxisVertsOffset) + 0] = cylinderStart;
        vertices[3*(i + xAxisVertsOffset) + 1] = coord2;
        vertices[3*(i + xAxisVertsOffset) + 2] = coord1;

        vertices[3*(i + sides + xAxisVertsOffset) + 0] = cylinderEnd;
        vertices[3*(i + sides + xAxisVertsOffset) + 1] = coord2;
        vertices[3*(i + sides + xAxisVertsOffset) + 2] = coord1;

        vertices[3*(i + yAxisVertsOffset) + 0] = coord1;
        vertices[3*(i + yAxisVertsOffset) + 1] = cylinderStart;
        vertices[3*(i + yAxisVertsOffset) + 2] = coord2;

        vertices[3*(i + sides + yAxisVertsOffset) + 0] = coord1;
        vertices[3*(i + sides + yAxisVertsOffset) + 1] = cylinderEnd;
        vertices[3*(i + sides + yAxisVertsOffset) + 2] = coord2;

        vertices[3*(i + zAxisVertsOffset) + 0] = coord2;
        vertices[3*(i + zAxisVertsOffset) + 1] = coord1;
        vertices[3*(i + zAxisVertsOffset) + 2] = cylinderStart;

        vertices[3*(i + sides + zAxisVertsOffset) + 0] = coord2;
        vertices[3*(i + sides + zAxisVertsOffset) + 1] = coord1;
        vertices[3*(i + sides + zAxisVertsOffset) + 2] = cylinderEnd;
    }

    // cone verts:
    const float coneRadius = s_coneRadiusPercentage;
    const int coneOffset = 2 * sides;

    for (int i = 0; i < 2*sides; ++i) {
        const float angle = (i * M_PI) / sides;
        const float coord1 = sin(angle) * coneRadius;
        const float coord2 = cos(angle) * coneRadius;

        vertices[3*(i + coneOffset + xAxisVertsOffset) + 0] = cylinderEnd;
        vertices[3*(i + coneOffset + xAxisVertsOffset) + 1] = coord2;
        vertices[3*(i + coneOffset + xAxisVertsOffset) + 2] = coord1;

        vertices[3*(i + coneOffset + yAxisVertsOffset) + 0] = coord1;
        vertices[3*(i + coneOffset + yAxisVertsOffset) + 1] = cylinderEnd;
        vertices[3*(i + coneOffset + yAxisVertsOffset) + 2] = coord2;

        vertices[3*(i + coneOffset + zAxisVertsOffset) + 0] = coord2;
        vertices[3*(i + coneOffset + zAxisVertsOffset) + 1] = coord1;
        vertices[3*(i + coneOffset + zAxisVertsOffset) + 2] = cylinderEnd;
    }
    {
        vertices[3*(2*sides + coneOffset + xAxisVertsOffset) + 0] = 1;
        vertices[3*(2*sides + coneOffset + xAxisVertsOffset) + 1] = 0;
        vertices[3*(2*sides + coneOffset + xAxisVertsOffset) + 2] = 0;

        vertices[3*(1 + 2*sides + coneOffset + xAxisVertsOffset) + 0] = cylinderEnd;
        vertices[3*(1 + 2*sides + coneOffset + xAxisVertsOffset) + 1] = 0;
        vertices[3*(1 + 2*sides + coneOffset + xAxisVertsOffset) + 2] = 0;

        vertices[3*(2*sides + coneOffset + yAxisVertsOffset) + 0] = 0;
        vertices[3*(2*sides + coneOffset + yAxisVertsOffset) + 1] = 1;
        vertices[3*(2*sides + coneOffset + yAxisVertsOffset) + 2] = 0;

        vertices[3*(1 + 2*sides + coneOffset + yAxisVertsOffset) + 0] = 0;
        vertices[3*(1 + 2*sides + coneOffset + yAxisVertsOffset) + 1] = cylinderEnd;
        vertices[3*(1 + 2*sides + coneOffset + yAxisVertsOffset) + 2] = 0;

        vertices[3*(2*sides + coneOffset + zAxisVertsOffset) + 0] = 0;
        vertices[3*(2*sides + coneOffset + zAxisVertsOffset) + 1] = 0;
        vertices[3*(2*sides + coneOffset + zAxisVertsOffset) + 2] = 1;

        vertices[3*(1 + 2*sides + coneOffset + zAxisVertsOffset) + 0] = 0;
        vertices[3*(1 + 2*sides + coneOffset + zAxisVertsOffset) + 1] = 0;
        vertices[3*(1 + 2*sides + coneOffset + zAxisVertsOffset) + 2] = cylinderEnd;
    }

    // cube verts:
    const int cubeVertsOffset = 3 * axisVerts;
    for (int i = 0; i < 8; ++i) {
        float signX = ((i % 4) > 0 && (i % 4) < 3) ? -1 : 1;
        vertices[3*(i + cubeVertsOffset) + 0] = signX * cylinderRadius;
        vertices[3*(i + cubeVertsOffset) + 1] = (i & 2) ? cylinderRadius : -cylinderRadius;
        vertices[3*(i + cubeVertsOffset) + 2] = (i & 4) ? cylinderRadius : -cylinderRadius;
    }


    // colors
    for (int i = 0; i < axisVerts; ++i) {
        color[3*i] = 1;

        color[3*(i + axisVerts) + 1] = 1;

        color[3*(i + 2*axisVerts) + 2] = 1;
    }
    for (int i = 0; i < 8; ++i) {
        color[3*(i + 3*axisVerts) + 0] = 0.5;
        color[3*(i + 3*axisVerts) + 1] = 0.5;
        color[3*(i + 3*axisVerts) + 2] = 0.5;
    }


    // indices
    const int xAxisElOffset = 0;
    const int yAxisElOffset = axisElements;
    const int zAxisElOffset = 2*axisElements;
    for (int i = 0; i < sides; ++i) {
        // cylinder elements
        indices[3*(2*i + xAxisElOffset) + 0] = i + xAxisVertsOffset;
        indices[3*(2*i + xAxisElOffset) + 1] = (i + 1) % sides + sides + xAxisVertsOffset;
        indices[3*(2*i + xAxisElOffset) + 2] = i + sides + xAxisVertsOffset;

        indices[3*(2*i + 1 + xAxisElOffset) + 0] = i + xAxisVertsOffset;
        indices[3*(2*i + 1 + xAxisElOffset) + 1] = (i + 1) % sides + xAxisVertsOffset;
        indices[3*(2*i + 1 + xAxisElOffset) + 2] = (i + 1) % sides + sides + xAxisVertsOffset;

        indices[3*(2*i + yAxisElOffset) + 0] = i + yAxisVertsOffset;
        indices[3*(2*i + yAxisElOffset) + 1] = (i + 1) % sides + sides + yAxisVertsOffset;
        indices[3*(2*i + yAxisElOffset) + 2] = i + sides + yAxisVertsOffset;

        indices[3*(2*i + 1 + yAxisElOffset) + 0] = i + yAxisVertsOffset;
        indices[3*(2*i + 1 + yAxisElOffset) + 1] = (i + 1) % sides + yAxisVertsOffset;
        indices[3*(2*i + 1 + yAxisElOffset) + 2] = (i + 1) % sides + sides + yAxisVertsOffset;

        indices[3*(2*i + zAxisElOffset) + 0] = i + zAxisVertsOffset;
        indices[3*(2*i + zAxisElOffset) + 1] = (i + 1) % sides + sides + zAxisVertsOffset;
        indices[3*(2*i + zAxisElOffset) + 2] = i + sides + zAxisVertsOffset;

        indices[3*(2*i + 1 + zAxisElOffset) + 0] = i + zAxisVertsOffset;
        indices[3*(2*i + 1 + zAxisElOffset) + 1] = (i + 1) % sides + zAxisVertsOffset;
        indices[3*(2*i + 1 + zAxisElOffset) + 2] = (i + 1) % sides + sides + zAxisVertsOffset;
    }

    for (int i = 0; i < 2*sides; ++i) {
        // cone elements
        indices[3*(i + coneOffset + xAxisElOffset) + 0] = i + coneOffset + xAxisVertsOffset;
        indices[3*(i + coneOffset + xAxisElOffset) + 1] = (i + 1) % (2*sides) + coneOffset + xAxisVertsOffset;
        indices[3*(i + coneOffset + xAxisElOffset) + 2] = 2*sides + coneOffset + xAxisVertsOffset;

        indices[3*(i + coneOffset + yAxisElOffset) + 0] = i + coneOffset + yAxisVertsOffset;
        indices[3*(i + coneOffset + yAxisElOffset) + 1] = (i + 1) % (2*sides) + coneOffset + yAxisVertsOffset;
        indices[3*(i + coneOffset + yAxisElOffset) + 2] = 2*sides + coneOffset + yAxisVertsOffset;

        indices[3*(i + coneOffset + zAxisElOffset) + 0] = i + coneOffset + zAxisVertsOffset;
        indices[3*(i + coneOffset + zAxisElOffset) + 1] = (i + 1) % (2*sides) + coneOffset + zAxisVertsOffset;
        indices[3*(i + coneOffset + zAxisElOffset) + 2] = 2*sides + coneOffset + zAxisVertsOffset;
    }

    const int capOffset = 4*sides;
    for (int i = 0; i < 2*sides; ++i) {
        // cone-cap elements
        indices[3*(i + capOffset + xAxisElOffset) + 0] = (i + 1) % (2*sides) + coneOffset + xAxisVertsOffset;
        indices[3*(i + capOffset + xAxisElOffset) + 1] = i + coneOffset + xAxisVertsOffset;
        indices[3*(i + capOffset + xAxisElOffset) + 2] = 1 + 2*sides + coneOffset + xAxisVertsOffset;

        indices[3*(i + capOffset + yAxisElOffset) + 0] = (i + 1) % (2*sides) + coneOffset + yAxisVertsOffset;
        indices[3*(i + capOffset + yAxisElOffset) + 1] = i + coneOffset + yAxisVertsOffset;
        indices[3*(i + capOffset + yAxisElOffset) + 2] = 1 + 2*sides + coneOffset + yAxisVertsOffset;

        indices[3*(i + capOffset + zAxisElOffset) + 0] = (i + 1) % (2*sides) + coneOffset + zAxisVertsOffset;
        indices[3*(i + capOffset + zAxisElOffset) + 1] = i + coneOffset + zAxisVertsOffset;
        indices[3*(i + capOffset + zAxisElOffset) + 2] = 1 + 2*sides + coneOffset + zAxisVertsOffset;
    }

    const int cubeElOffset = 3*axisElements;
    {
        // cube elements
        indices[3*(0 + cubeElOffset) + 0] = 0 + cubeVertsOffset;
        indices[3*(0 + cubeElOffset) + 1] = 1 + cubeVertsOffset;
        indices[3*(0 + cubeElOffset) + 2] = 2 + cubeVertsOffset;

        indices[3*(1 + cubeElOffset) + 0] = 0 + cubeVertsOffset;
        indices[3*(1 + cubeElOffset) + 1] = 2 + cubeVertsOffset;
        indices[3*(1 + cubeElOffset) + 2] = 3 + cubeVertsOffset;

        indices[3*(2 + cubeElOffset) + 0] = 4 + cubeVertsOffset;
        indices[3*(2 + cubeElOffset) + 1] = 6 + cubeVertsOffset;
        indices[3*(2 + cubeElOffset) + 2] = 5 + cubeVertsOffset;

        indices[3*(3 + cubeElOffset) + 0] = 4 + cubeVertsOffset;
        indices[3*(3 + cubeElOffset) + 1] = 7 + cubeVertsOffset;
        indices[3*(3 + cubeElOffset) + 2] = 6 + cubeVertsOffset;

        indices[3*(4 + cubeElOffset) + 0] = 3 + cubeVertsOffset;
        indices[3*(4 + cubeElOffset) + 1] = 6 + cubeVertsOffset;
        indices[3*(4 + cubeElOffset) + 2] = 7 + cubeVertsOffset;

        indices[3*(5 + cubeElOffset) + 0] = 3 + cubeVertsOffset;
        indices[3*(5 + cubeElOffset) + 1] = 2 + cubeVertsOffset;
        indices[3*(5 + cubeElOffset) + 2] = 6 + cubeVertsOffset;

        indices[3*(6 + cubeElOffset) + 0] = 0 + cubeVertsOffset;
        indices[3*(6 + cubeElOffset) + 1] = 4 + cubeVertsOffset;
        indices[3*(6 + cubeElOffset) + 2] = 5 + cubeVertsOffset;

        indices[3*(7 + cubeElOffset) + 0] = 0 + cubeVertsOffset;
        indices[3*(7 + cubeElOffset) + 1] = 5 + cubeVertsOffset;
        indices[3*(7 + cubeElOffset) + 2] = 1 + cubeVertsOffset;

        indices[3*(8 + cubeElOffset) + 0] = 6 + cubeVertsOffset;
        indices[3*(8 + cubeElOffset) + 1] = 2 + cubeVertsOffset;
        indices[3*(8 + cubeElOffset) + 2] = 1 + cubeVertsOffset;

        indices[3*(9 + cubeElOffset) + 0] = 6 + cubeVertsOffset;
        indices[3*(9 + cubeElOffset) + 1] = 1 + cubeVertsOffset;
        indices[3*(9 + cubeElOffset) + 2] = 5 + cubeVertsOffset;

        indices[3*(10 + cubeElOffset) + 0] = 7 + cubeVertsOffset;
        indices[3*(10 + cubeElOffset) + 1] = 0 + cubeVertsOffset;
        indices[3*(10 + cubeElOffset) + 2] = 3 + cubeVertsOffset;

        indices[3*(11 + cubeElOffset) + 0] = 7 + cubeVertsOffset;
        indices[3*(11 + cubeElOffset) + 1] = 4 + cubeVertsOffset;
        indices[3*(11 + cubeElOffset) + 2] = 0 + cubeVertsOffset;
    }
}

void AxisGnomon::updateProjectionMatrix()
{
    // Specify the arguments to QMatrix4x4::ortho() in OpenGL window coords. Using the
    // QRect/QRectF overloads is no-intuitive and flips the y-axis if you get it wrong.
    // This is clearer and works. It means that the m_position variable is
    // specified in OpenGL window coordinates (origin at lower left of window).
    QMatrix4x4 orthProjection;
    orthProjection.ortho(0.0f, m_windowSize.width(), 0.0f, m_windowSize.height(), -1.0f, 1.0f);

    QMatrix4x4 position;
    position.translate(m_position.x(), m_position.y());
    position.scale(m_size, m_size, 1);

    m_projection = orthProjection * position;
}
