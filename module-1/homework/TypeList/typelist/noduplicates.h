#pragma once

#include "erase.h"
#include "typelist.h"

template<typename TList>
struct NoDuplicates;

template <>
struct NoDuplicates<NullType>
{
    typedef NullType NewTypeList;
};

template <typename Head, typename Tail>
struct NoDuplicates<TypeList<Head, Tail>>
{
private:
    typedef typename NoDuplicates<Tail>::NewTypeList L1;
    typedef typename Erase<L1, Head>::NewTypeList L2;

public:
    typedef TypeList<Head, L2> NewTypeList;
};
