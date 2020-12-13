#pragma once

template <typename T, typename U>
struct TypeList
{
    typedef T Head;
    typedef U Tail;
};

struct NullType
{ };
