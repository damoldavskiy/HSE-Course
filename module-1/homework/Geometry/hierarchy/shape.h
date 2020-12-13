#pragma once

#include "line.h"

class Shape
{
public:
    virtual double perimeter() const = 0;
    virtual double area() const = 0;

    virtual bool operator ==(const Shape& another) const = 0;
    virtual bool operator !=(const Shape& another) const;
    virtual bool isCongruentTo(const Shape& another) const = 0;
    virtual bool isSimilarTo(const Shape& another) const = 0;
    virtual bool containsPoint(const Point& point) const = 0;

    virtual void shift(const Point& center) = 0;
    virtual void rotate(const Point& center, double angle) = 0;
    virtual void reflex(const Point& center) = 0;
    virtual void reflex(const Line& axis) = 0;
    virtual void scale(const Point& center, double coefficient) = 0;
};
