#pragma once

#include "shape.h"

#include <vector>

class Polygon : public Shape
{
public:
    Polygon(const std::vector<Point>& points);

    int verticesCount() const { return _points.size(); }
    std::vector<Point> getVertices() const { return _points; }

    bool isConvex() const;

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
    std::vector<Point> _points;
};
