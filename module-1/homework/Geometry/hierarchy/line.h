#pragma once

#include "point.h"

struct Line
{
    Point p1;
    Point p2;

    Line(Point p1, Point p2);

    Point vector() const;
    bool contains(const Point& point) const;

    static double angle(const Line& lhs, const Line& rhs);
    static void reflex(Point& point, const Line& axis);
};

bool operator ==(const Line& lhs, const Line& rhs);
bool operator !=(const Line& lhs, const Line& rhs);
