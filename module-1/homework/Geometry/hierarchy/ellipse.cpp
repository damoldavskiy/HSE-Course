#include "ellipse.h"

#include "constants.h"

#include <cmath>
#include <tuple>

Ellipse::Ellipse(const Point& f1, const Point& f2, double d)
    : _focus1(f1), _focus2(f2), _a(d / 2)
{
    if (d <= 0)
        throw std::invalid_argument("D parameter must be positive");
}

double Ellipse::a() const
{
    return _a;
}

double Ellipse::b() const
{
    return sqrt(pow(_a, 2) - pow(_a * eccentricity(), 2));
}

std::pair<Line, Line> Ellipse::directrices() const
{
    Point c = center();
    double e = eccentricity();

    Point d1 = Line { c, _focus1 }.vector();
    Point p1 = c;
    p1.shift({ d1.x / d1.abs() * _a / e, d1.y / d1.abs() * _a / e });

    Point d2 = Line { c, _focus2 }.vector();
    Point p2 = c;
    p2.shift({ d2.x / d2.abs() * _a / e, d1.y / d2.abs() * _a / e });

    Line l1 { _focus1, c };
    Line l2 { _focus2, c };

    l1.p1.rotate(p1, PI / 2);
    l2.p1.rotate(p2, PI / 2);

    return { l1, l2 };
}

double Ellipse::eccentricity() const
{
    return Point::distance(center(), _focus1) / a();
}

Point Ellipse::center() const
{
    return { (_focus2.x + _focus1.x) / 2, (_focus2.y + _focus1.y) / 2 };
}

double Ellipse::perimeter() const
{
    return 4 * (PI * a() * b() + pow(a() - b(), 2)) / (a() + b());
}

double Ellipse::area() const
{
    return a() * b() * PI;
}

bool Ellipse::operator ==(const Shape& another) const
{
    const Ellipse* ellipse = dynamic_cast<const Ellipse*>(&another);
    if (ellipse == nullptr)
        return false;

    return _focus1 == ellipse->_focus1 && _focus2 == ellipse->_focus2 && _a == ellipse->_a;
}

bool Ellipse::isCongruentTo(const Shape& another) const
{
    const Ellipse* ellipse = dynamic_cast<const Ellipse*>(&another);
    if (ellipse == nullptr)
        return false;

    return a() == ellipse->a() && b() == ellipse->b();
}

bool Ellipse::isSimilarTo(const Shape& another) const
{
    const Ellipse* ellipse = dynamic_cast<const Ellipse*>(&another);
    if (ellipse == nullptr)
        return false;

    if (a() / b() != ellipse->a() / ellipse->b())
        return false;

    return Line::angle({ _focus1, _focus2 }, { ellipse->_focus1, ellipse->_focus2 }) == 0;
}

bool Ellipse::containsPoint(const Point& point) const
{
    return Point::distance(_focus1, point) + Point::distance(_focus2, point) <= 2 * _a;
}

void Ellipse::shift(const Point& center)
{
    _focus1.shift(center);
    _focus2.shift(center);
}

void Ellipse::rotate(const Point& center, double angle)
{
    _focus1.rotate(center, angle);
    _focus2.rotate(center, angle);
}

void Ellipse::reflex(const Point& center)
{
    _focus1.reflex(center);
    _focus2.reflex(center);
}

void Ellipse::reflex(const Line& axis)
{
    Line::reflex(_focus1, axis);
    Line::reflex(_focus2, axis);
}

void Ellipse::scale(const Point& center, double coefficient)
{
    double oldF = Point::distance(this->center(), _focus1);

    _focus1.scale(center, coefficient);
    _focus2.scale(center, coefficient);

    double newF = Point::distance(this->center(), _focus1);
    _a *= newF / oldF;
}
