#include "logicalplane.h"

#include <QDebug>

LogicalPlane::LogicalPlane() :
    m_d(0.0f)
{
}

LogicalPlane::LogicalPlane(const QVector3D &pos, const QVector3D &normal) :
    m_normal(normal.normalized())
{
    m_d = QVector3D::dotProduct(pos, normal);
}

LogicalPlane::LogicalPlane(const QVector3D &p0, const QVector3D &p1, const QVector3D &p2)
{
    QVector3D u = p1 - p0, v = p2 - p0;
    m_normal = QVector3D::crossProduct(u, v).normalized();
    m_d = QVector3D::dotProduct(p0, m_normal);
}

LogicalPlane::LogicalPlane(const QVector4D &planeEquation)
{
    m_normal = planeEquation.toVector3D();
// see link for derivation of this
// http://mathworld.wolfram.com/HessianNormalForm.html
    m_d = planeEquation.w() / m_normal.length();
    m_normal.normalize();
}

float LogicalPlane::distance(const QVector3D &p) const
{
    return QVector3D::dotProduct(p, m_normal) + m_d;
}

LogicalPlane LogicalPlane::mirrored() const
{
    LogicalPlane result;

    result.m_normal = -m_normal;
    result.m_d = -m_d;

    return result;
}

bool LogicalPlane::intersectLine(const QVector3D &a, const QVector3D &b, QVector3D &result) const
{
    // Realtime collision detection, p176
    QVector3D ab = b - a;
    float t = (m_d - QVector3D::dotProduct(m_normal, a)) / QVector3D::dotProduct(m_normal, ab);
    if ((t >= 0.0f) && (t <= 1.0f)) {
        result = a + (t * ab);
        return true;
    }

    return false;
}

QDebug operator<<(QDebug stream, const LogicalPlane &p)
{
    stream << "half-space: n=" << p.normal() << ", w=" << p.w();
    return stream;
}
