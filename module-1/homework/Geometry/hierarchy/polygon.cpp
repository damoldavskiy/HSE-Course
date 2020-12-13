#include "polygon.h"

#include "constants.h"

#include <algorithm>
#include <stdexcept>
#include <set>
#include <tuple>

Polygon::Polygon(const std::vector<Point>& points)
    : _points(points)
{
    if (points.size() < 3)
        throw std::invalid_argument("At least 3 points required");
}

bool Polygon::isConvex() const
{
    Point a = Line { _points.back(), _points.front() }.vector();
    Point b = Line { _points.front(), _points[1] }.vector();

    int sign = a.x * b.y - a.y * b.x;

    bool neg = sign < 0;
    bool pos = sign > 0;

    for (size_t i = 1; i < _points.size() - 1; ++i) {
        a = Line { _points[i - 1], _points[i] }.vector();
        b = Line { _points[i], _points[i + 1] }.vector();

        sign = a.x * b.y - a.y * b.x;
        neg |= sign < 0;
        pos |= sign > 0;
    }

    return pos ^ neg;
}

double Polygon::perimeter() const
{
    double sum = 0;

    for (size_t i = 0; i < _points.size(); ++i)
        sum += Point::distance(_points[i], _points[(i + 1) % _points.size()]);

    return sum;
}

double Polygon::area() const
{
    double sum = 0;

    for (size_t i = 0; i < _points.size(); ++i) {
        sum += _points[i].x * _points[(i + 1) % _points.size()].y;
        sum -= _points[i].y * _points[(i + 1) % _points.size()].x;
    }

    return std::abs(sum) / 2 ;
}

bool Polygon::operator ==(const Shape& another) const
{
    const Polygon* polygon = dynamic_cast<const Polygon*>(&another);
    if (polygon == nullptr)
        return false;

    auto comp = [] (const Line& lhs, const Line& rhs) {
        if (lhs == rhs)
            return false;
        return std::tie(lhs.p1.x, lhs.p1.y, lhs.p2.x, lhs.p2.y)
                   < std::tie(rhs.p1.x, rhs.p1.y, rhs.p2.x, rhs.p2.y);
    };

    std::set<Line, decltype(comp)> sides(comp);

    for (size_t i = 0; i < _points.size(); ++i)
        sides.insert({ _points[i], _points[(i + 1) % _points.size()] });

    for (size_t i = 0; i < polygon->_points.size(); ++i) {
        Point p1 = polygon->_points[i];
        Point p2 = polygon->_points[(i + 1) % polygon->_points.size()];

        std::set<Line>::iterator it;
        if ((it = sides.find({ p1, p2 })) != sides.end() ||
                (it = sides.find({ p2, p1 })) != sides.end())
            sides.erase(it);
        else
            return false;
    }

    return sides.empty();
}

bool Polygon::isCongruentTo(const Shape& another) const
{
    const Polygon* polygon = dynamic_cast<const Polygon*>(&another);
    if (polygon == nullptr)
        return false;

    if (_points.size() != polygon->_points.size())
        return false;

    size_t n = _points.size();
    for (size_t start = 0; start < n; ++start) {
        for (size_t i1 = 0; i1 < n; ++i1) {
            size_t i2 = (i1 + 1) % n;
            size_t j1 = (i1 + start) % n;
            size_t j2 = (j1 + 1) % n;

            Line l1 { _points[i1], _points[i2] };
            Line r1 { polygon->_points[j1], polygon->_points[j2] };

            if (std::abs(l1.vector().abs() - r1.vector().abs()) >= EPS)
                break;

            size_t i3 = (i2 + 1) % n;
            size_t j3 = (j2 + 1) % n;

            Line l2 { _points[i2], _points[i3] };
            Line r2 { polygon->_points[j2], polygon->_points[j3] };

            if (std::abs(Line::angle(l1, l2) - Line::angle(r1, r2)) >= EPS)
                break;

            if (i1 == n - 1)
                return true;
        }
    }

    return false;
}

bool Polygon::isSimilarTo(const Shape& another) const
{
    const Polygon* polygon = dynamic_cast<const Polygon*>(&another);
    if (polygon == nullptr)
        return false;

    if (_points.size() != polygon->_points.size())
        return false;

    double min1 = Point::distance(_points[0], _points[1]);
    double min2 = Point::distance(polygon->_points[0], polygon->_points[1]);

    for (size_t i = 0; i < _points.size(); ++i) {
        Point p11 = _points[i];
        Point p12 = _points[(i + 1) % polygon->_points.size()];

        Point p21 = polygon->_points[i];
        Point p22 = polygon->_points[(i + 1) % polygon->_points.size()];

        min1 = std::min(min1, Point::distance(p11, p12));
        min2 = std::min(min2, Point::distance(p21, p22));
    }

    Polygon second = *polygon;
    second.scale({ 0, 0 }, min1 / min2);

    return isCongruentTo(second);
}

bool Polygon::containsPoint(const Point& point) const
{
    int sides = 0;
    int points = 0;

    for (size_t i = 0; i < _points.size(); ++i) {
        Point p1 = _points[i];
        Point p2 = _points[(i + 1) % _points.size()];

        int sign = 1;
        if ((p2.x - p1.x) * (p2.y - p1.y) < 0)
            sign = -1;

        if (sign * (point.x - p1.x) / (p2.x - p1.x) >= sign * (point.y - p1.y) / (p2.y - p1.y)) {
            if (std::abs(p1.x - point.x) < EPS || std::abs(p2.x - point.x) < EPS) {
                ++sides;
                ++points;
            } else if ((p1.x < point.x) ^ (p2.x < point.x)) {
                ++sides;
            }
        }
    }

    return (sides - points) % 2 == 1;
}


void Polygon::shift(const Point& center)
{
    for (Point& point : _points)
        point.shift(center);
}

void Polygon::rotate(const Point& center, double angle)
{
    for (Point& point : _points)
        point.rotate(center, angle);
}

void Polygon::reflex(const Point& center)
{
    for (Point& point : _points)
        point.reflex(center);
}

void Polygon::reflex(const Line& axis)
{
    for (Point& point : _points)
        Line::reflex(point, axis);
}

void Polygon::scale(const Point& center, double coefficient)
{
    for (Point& point : _points)
        point.scale(center, coefficient);
}
