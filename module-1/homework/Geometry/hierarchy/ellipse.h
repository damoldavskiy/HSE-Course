#pragma once

#include "shape.h"

class Ellipse : public Shape
{
public:
    Ellipse(const Point& _focus1, const Point& _focus2, double _d);

    std::pair<Point, Point> focuses() const { return { _focus1, _focus2 }; }

    double a() const;
    double b() const;
    std::pair<Line, Line> directrices() const;
    double eccentricity() const;
    Point center() const;

    virtual double perimeter() const;
    virtual double area() const;

    virtual bool operator ==(const Shape& another) const;
    virtual bool isCongruentTo(const Shape& another) const;
    virtual bool isSimilarTo(const Shape& another) const;
    virtual bool containsPoint(const Point& point) const;

    virtual void shift(const Point& center);
    virtual void rotate(const Point& center, double angle);
    virtual void reflex(const Point& center);
    virtual void reflex(const Line& axis);
    virtual void scale(const Point& center, double coefficient);

protected:
    Point _focus1;
    Point _focus2;
    double _a;
};
