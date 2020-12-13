#include "square.h"

Square::Square(const Point& p1, const Point& p2)
    : Rectangle(p1, p2, 1)
{ }

Circle Square::circumscribedCircle() const
{
    return Circle(center(), Point::distance(_points[0], _points[2]) / 2);
}

Circle Square::inscribedCircle() const
{
    return Circle(center(), Point::distance(_points[0], _points[1]) / 2);
}
