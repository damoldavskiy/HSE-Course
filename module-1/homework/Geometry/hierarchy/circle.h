#pragma once

#include "ellipse.h"

class Circle : public Ellipse
{
public:
    Circle(const Point& center, double radius);

    double radius() const;
};
