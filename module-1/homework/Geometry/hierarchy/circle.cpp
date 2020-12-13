#include "circle.h"

Circle::Circle(const Point& center, double radius)
    : Ellipse(center, center, radius * 2)
{ }

double Circle::radius() const
{
    return _a;
}
