#pragma once

#include "typelist.h"

template<typename TList, typename OldType, typename NewType>
struct Replace;

template <typename T, typename U>
struct Replace<NullType, T, U>
{
    typedef NullType NewTypeList;
};

template <typename T, typename Tail, typename U>
struct Replace<TypeList<T, Tail>, T, U>
{
    typedef TypeList<U, Tail> NewTypeList;
};

template  <typename Head, typename Tail, typename T, typename U>
struct Replace<TypeList<Head, Tail>, T, U>
{
    typedef TypeList<Head, typename Replace<Tail, T, U>::NewTypeList> NewTypeList;
};
