#include "shape.h"

bool Shape::operator !=(const Shape& another) const
{
    return !(*this == another);
}

