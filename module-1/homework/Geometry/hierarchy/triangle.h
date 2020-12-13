#pragma once

#include "circle.h"
#include "polygon.h"

class Triangle : public Polygon
{
public:
    Triangle(const Point& p1, const Point& p2, const Point& p3);

    Circle circumscribedCircle() const;
    Circle inscribedCircle() const;
};
