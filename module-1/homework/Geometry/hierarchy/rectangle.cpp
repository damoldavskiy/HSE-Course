#include "rectangle.h"

#include "constants.h"

#include <cmath>

namespace {

std::vector<Point> rectanglePoints(const Point& p1, const Point& p3, double f)
{
    double d = Point::distance(p1, p3);
    double a = sqrt(pow(d, 2) / (1 + pow(f, 2)));
    double b = a * f;

    Point c = { (p1.x + p3.x) / 2, (p1.y + p3.y) / 2 };

    double cosa = a / d;
    double sina = b / d;
    double base = cosa * a;
    double h = sina * a;

    Point basePoint = p1;
    Point dir = Line { p1, p3 }.vector();
    basePoint.shift({ dir.x / dir.abs() * base, dir.y / dir.abs() * base });

    Point p2 = basePoint;
    p2.shift({ dir.x / dir.abs() * h, dir.y / dir.abs() * h });
    p2.rotate(basePoint, 90);

    Point p4 = p2;
    dir = Line { p2, c }.vector();
    p4.shift({ dir.x / dir.abs() * d, dir.y / dir.abs() * d });

    return { p1, p2, p3, p4 };
}

}

Rectangle::Rectangle(const Point& p1, const Point& p2, double factor)
    : Polygon(rectanglePoints(p1, p2, factor))
{ }

Point Rectangle::center() const
{
    return { (_points[0].x + _points[2].x) / 2, (_points[0].y + _points[1].y) / 2 };
}

std::pair<Line, Line> Rectangle::diagonals() const
{
    return {
        { _points[0], _points[2] },
        { _points[1], _points[3] }
    };
}
