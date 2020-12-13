#include "line.h"

#include "constants.h"

#include <cmath>

Line::Line(Point p1, Point p2)
    : p1(p1), p2(p2)
{ }

Point Line::vector() const
{
    return { p2.x - p1.x, p2.y - p1.y };
}

bool Line::contains(const Point& point) const
{
    return point == p1 || point == p2
               || std::abs((point.x - p1.x) / (p2.x - p1.x) - (point.y - p1.y) / (p2.y - p1.y)) < EPS;
}

double Line::angle(const Line& lhs, const Line& rhs)
{
    return Point::angle(lhs.vector(), rhs.vector());
}

void Line::reflex(Point &point, const Line &axis)
{
    double p1x = point.x;
    double p1y = point.y;
    double vx1 = axis.p1.x;
    double vy1 = axis.p1.y;
    double vx2 = axis.p2.x;
    double vy2 = axis.p2.y;

    double A = (vy2 - vy1) / (vx2 - vx1);
    double B = (vx2 - vx1) / (vy2 - vy1);
    double C = p1y - vy1;

    double p2x = (C + p1x * B + vx1 * A) / (A + B);
    double p2y = p1y - (p2x - p1x) * B;

    Point dir = Line { point, { p2x, p2y } }.vector();
    point.shift({ dir.x * 2, dir.y * 2 });
}

bool operator ==(const Line& lhs, const Line& rhs)
{
    return lhs.contains(rhs.p1) && lhs.contains(rhs.p2);
}

bool operator !=(const Line& lhs, const Line& rhs)
{
    return !(lhs == rhs);
}
