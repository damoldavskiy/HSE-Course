#pragma once

#include <utility>

struct Point
{
    double x;
    double y;

    Point(double x, double y);

    double abs() const;

    void shift(const Point& center);
    void rotate(const Point& center, double angle);
    void reflex(const Point& center);
    void scale(const Point& center, double coefficient);

    static double angle(const Point& lhs, const Point& rhs);
    static double distance(const Point& lhs, const Point& rhs);
};

bool operator ==(const Point& lhs, const Point& rhs);
bool operator !=(const Point& lhs, const Point& rhs);
