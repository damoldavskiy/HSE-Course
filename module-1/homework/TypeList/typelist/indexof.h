#pragma once

#include "typelist.h"

template <typename TList, typename TargetType>
struct IndexOf;

template <typename T>
struct IndexOf<NullType, T>
{
    enum { pos = -1 };
};

template <typename T, typename Tail>
struct IndexOf<TypeList<T, Tail>, T>
{
    enum { pos = 0 };
};

template <typename Head, typename Tail, typename T>
struct IndexOf<TypeList<Head, Tail>, T>
{
private:
    enum { tail = IndexOf<Tail, T>::pos };

public:
    enum { pos = tail == -1 ? -1 : tail + 1 };
};
