#pragma once

#include "typelist.h"

template<typename TList, typename TargetType>
struct Erase;

template <typename T>
struct Erase<NullType, T>
{
    typedef NullType NewTypeList;
};

template <typename T, typename Tail>
struct Erase<TypeList<T, Tail>, T>
{
    typedef Tail NewTypeList;
};

template <typename Head, typename Tail, typename T>
struct Erase<TypeList<Head, Tail>, T>
{
    typedef TypeList<Head, typename Erase<Tail, T>::NewTypeList> NewTypeList;
};
