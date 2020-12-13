#pragma once

#include "circle.h"
#include "rectangle.h"

class Square : public Rectangle
{
    Square(const Point& p1, const Point& p2);

    Circle circumscribedCircle() const;
    Circle inscribedCircle() const;
};
