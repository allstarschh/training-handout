#ifndef LOGICALPLANE_H
#define LOGICALPLANE_H

#include <QVector3D>
#include <QVector4D>

/**
 * @brief A geometric plane (half-space). Stored as unit normal + distance
 * from the origin, see http://mathworld.wolfram.com/HessianNormalForm.html
 * for example of this format.
 */
class LogicalPlane
{
public:
    LogicalPlane();

    /**
     * @brief construct from a normal and point on the plane
     * @param pos
     * @param normal
     */
    LogicalPlane(const QVector3D& pos, const QVector3D& normal);

    /**
     * @brief construct from three points
     * @param p0
     * @param p1
     * @param p2
     */
    LogicalPlane(const QVector3D& p0, const QVector3D& p1,
                 const QVector3D& p2);

    /**
     * @brief LogicalPlane - create a plane based on the traditional
     * parametric equation (a, b, c, d) expressed as a 4D vector
     * @param planeEquation - vector containing the parametric coefficients
     */
    explicit LogicalPlane(const QVector4D& planeEquation);

    /**
     * @brief distance of a point to the plane
     * @param p
     * @return
     */
    float distance(const QVector3D& p) const;

    QVector3D normal() const
    { return m_normal; }

    float w() const
    { return m_d; }

    LogicalPlane mirrored() const;

    /**
     * @brief Intersection between a line segment and the plane
     * @return true if an intersection exists, false otherwise
     *
     */
    bool intersectLine(const QVector3D& a, const QVector3D& b, QVector3D& result) const;

private:
    float m_d;
    QVector3D m_normal;
};

QDebug operator<<(QDebug stream, const LogicalPlane& p);


#endif // LOGICALPLANE_H
