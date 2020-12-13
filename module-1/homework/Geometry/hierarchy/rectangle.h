#pragma once

#include "polygon.h"

class Rectangle : public Polygon
{
public:
    Rectangle(const Point& p1, const Point& p2, double factor);

    Point center() const;
    std::pair<Line, Line> diagonals() const;
};
