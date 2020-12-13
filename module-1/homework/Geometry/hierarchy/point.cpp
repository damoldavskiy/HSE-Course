#include "point.h"

#include "constants.h"

#include <cmath>

Point::Point(double x, double y)
    : x(x), y(y)
{ }

double Point::abs() const
{
    return sqrt(pow(x, 2) + pow(y, 2));
}

void Point::shift(const Point& center)
{
    x += center.x;
    y += center.y;
}

void Point::rotate(const Point& center, double angle)
{
    angle *= PI / 180;

    double r = distance(*this, center);

    if (r < EPS)
        return;

    double dx = x - center.x;
    double dy = y - center.y;

    double curCos = dx / r;
    double curSin = dy / r;

    double curAngle = acos(curCos);
    if (curSin < 0)
        curAngle = 2 * PI - curAngle;

    curAngle += angle;

    x = center.x + cos(curAngle) * r;
    y = center.y + sin(curAngle) * r;
}

void Point::reflex(const Point& center)
{
    x = 2 * center.x - x;
    y = 2 * center.y - y;
}

void Point::scale(const Point& center, double coefficient)
{
    x = center.x + (x - center.x) * coefficient;
    y = center.y + (y - center.y) * coefficient;
}

double Point::angle(const Point& lhs, const Point& rhs)
{
    return acos((lhs.x * rhs.x + lhs.y * rhs.y) / sqrt(pow(lhs.x, 2) + pow(rhs.y, 2))) * 180 / PI;
}

double Point::distance(const Point& lhs, const Point& rhs)
{
    return sqrt(pow(rhs.x - lhs.x, 2) + pow(rhs.y - lhs.y, 2));
}

bool operator ==(const Point& lhs, const Point& rhs)
{
    return std::abs(lhs.x - rhs.x) < EPS && std::abs(lhs.y - rhs.y) < EPS;
}

bool operator !=(const Point& lhs, const Point& rhs)
{
    return !(lhs == rhs);
}
