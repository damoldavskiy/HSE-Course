#include "triangle.h"

#include <cmath>

Triangle::Triangle(const Point& p1, const Point& p2, const Point& p3)
    : Polygon({ p1, p2, p3 })
{ }

Circle Triangle::circumscribedCircle() const
{
    const Point& a = _points[0];
    const Point& b = _points[1];
    const Point& c = _points[2];

    double d = 2 * (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y));

    double x = ((pow(a.x, 2) + pow(a.y, 2)) * (b.y - c.y) + (pow(b.x, 2) + pow(b.y, 2)) * (c.y - a.y) + (pow(c.x, 2) + pow(c.y, 2)) * (a.y - b.y)) / d;
    double y = ((pow(a.x, 2) + pow(a.y, 2)) * (c.x - b.x) + (pow(b.x, 2) + pow(b.y, 2)) * (a.x - c.x) + (pow(c.x, 2) + pow(c.y, 2)) * (b.x - a.x)) / d;

    return Circle({ x, y }, Point::distance({ x, y }, a));
}

Circle Triangle::inscribedCircle() const
{
    double r = 2 * area() / perimeter();
    double x = perimeter() / 2 - Point::distance(_points[1], _points[2]);

    Point basePoint = _points[0];
    Point dir = Line { _points[0], _points[1] }.vector();
    basePoint.shift({ dir.x / dir.abs() * x, dir.y / dir.abs() * x });

    Point c1 = basePoint;
    c1.shift({ dir.x / dir.abs() * r, dir.y / dir.abs() * r });
    c1.rotate(basePoint, 90);

    Point c2 = basePoint;
    c2.shift({ -dir.x / dir.abs() * r, -dir.y / dir.abs() * r });
    c2.rotate(basePoint, 90);

    return containsPoint(c1) ? Circle(c1, r) : Circle(c2, r);
}
