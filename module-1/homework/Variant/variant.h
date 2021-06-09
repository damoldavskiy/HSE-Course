#include <cstdlib>
#include <utility>

#pragma once

namespace task {

template <std::size_t I>
struct InPlaceIndex {
    explicit InPlaceIndex() = default;
};

template <std::size_t I>
constexpr InPlaceIndex<I> kInPlaceIndex{};

template <std::size_t Idx, typename... Types>
union UnionType;

template <std::size_t Idx>
union UnionType<Idx> {};

template <std::size_t Idx, typename T, typename... Types>
union UnionType<Idx, T, Types...> {
public:
    T head;
    UnionType<Idx + 1, Types...> tail;

    UnionType() : tail() {
        if (!std::is_trivially_constructible_v<T>) {
            new (&head) T();
        }
    }

    ~UnionType() {
        if (!std::is_trivially_destructible_v<T>) {
            head.~T();
        }
    }

    friend struct AccessUnion;
};

struct AssignUnion {
    template <std::size_t Idx, typename U, std::size_t UnIdx, typename Head, typename... Tail>
    static void AssignHelper(U&& value, InPlaceIndex<0>, UnionType<UnIdx, Head, Tail...>& u) {
        u.head = value;
    }

    template <std::size_t Idx, typename U, std::size_t UnIdx, typename Head, typename... Tail>
    static void AssignHelper(U&& value, InPlaceIndex<Idx>, UnionType<UnIdx, Head, Tail...>& u) {
        AssignUnion::AssignHelper<Idx - 1>(std::forward<U>(value), kInPlaceIndex<Idx - 1>, u.tail);
    }
};

struct AccessUnion {
    template <typename U>
    static constexpr auto&& GetAlt(U&& v, InPlaceIndex<0>) {
        return std::forward<U>(v).head;
    }

    template <typename U, std::size_t Idx>
    static constexpr auto&& GetAlt(U&& v, InPlaceIndex<Idx>) {
        return GetAlt(std::forward<U>(v).tail, kInPlaceIndex<Idx - 1>);
    }
};

struct VariantHelper {
    template <std::size_t Idx, typename T>
    static constexpr auto&& GetAlt(T&& v) {
        return AccessUnion::GetAlt(std::forward<T>(v).data_, kInPlaceIndex<Idx>);
    }
};

template <typename Head, typename... Tail>
struct Typelist {
    typedef Head head;
    typedef Typelist<Tail...> tail;
};

template <unsigned int Idx, class TList>
struct TypeAt;

template <typename Head, typename... Tail>
struct TypeAt<0, Typelist<Head, Tail...>> {
    typedef Head result;
};

template <unsigned int Idx, typename Head, typename... Tail>
struct TypeAt<Idx, Typelist<Head, Tail...>> {
    typedef typename TypeAt<Idx - 1, Typelist<Tail...>>::result result;
};

const static std::size_t kNotFounded = -1;
const static std::size_t kAmbiguity = kNotFounded - 1;

constexpr std::size_t ProcessBackward(std::size_t i, std::size_t res, const bool* founded,
                                      const bool* founded_convertible) {
    if (res == kAmbiguity) {
        return res;
    }

    if (founded[i]) {
        if (res == kNotFounded || !(!founded[res] && founded_convertible[res])) {
            return i;
        }
        return kAmbiguity;
    }

    if (res == kNotFounded && founded_convertible[i]) {
        return i;
    }

    return res;
}

template <std::size_t SizeofFounded>
constexpr std::size_t ProcessForeward(std::size_t cur_pos, const bool (&founded)[SizeofFounded],
                                      const bool (&convertible)[SizeofFounded]) {
    if (cur_pos == SizeofFounded) {
        return kNotFounded;
    }
    return ProcessBackward(cur_pos, ProcessForeward(cur_pos + 1, founded, convertible), founded,
                           convertible);
}

template <typename TargetType, typename... Types>
struct FindExactlyOneChecked {
    constexpr static bool kFounded[sizeof...(Types)] = {std::is_same<TargetType, Types>::value...};
    constexpr static bool kFoundedConvertible[sizeof...(Types)] = {
        std::is_convertible<TargetType, Types>::value...};
    constexpr static std::size_t kValue = ProcessForeward(0, kFounded, kFoundedConvertible);

    static_assert(kValue != kNotFounded, "type not found in type list");
    static_assert(kValue != kAmbiguity, "type occurs more than once in type list");
};

template <typename T>
struct FindExactlyOneChecked<T> {
    static_assert(!std::is_same<T, T>::value, "type not in empty type list");
};

template <typename TargetType, typename... Types>
struct FindExactlyOne : public FindExactlyOneChecked<TargetType, Types...> {};

template <typename... Types>
class Variant;

template <size_t Idx, typename T>
struct VariantAlternative;

template <size_t Idx, typename T>
using variant_alternative_t = typename VariantAlternative<Idx, T>::type;

template <size_t Idx, typename... Types>
struct VariantAlternative<Idx, Variant<Types...>> {
    using type = typename TypeAt<Idx, Typelist<Types...>>::result;
};

template <typename... Types>
class Variant {
public:
    // Special member functions
    constexpr Variant() noexcept;

    template <typename T, std::size_t Pos = FindExactlyOne<T, Types...>::kValue>
    Variant& operator=(T&& t) noexcept;

private:
    UnionType<0, Types...> data_;

    friend VariantHelper;
};

template <typename... Types>
constexpr Variant<Types...>::Variant() noexcept {
}

template <typename... Types>
template <typename T, std::size_t Pos>
Variant<Types...>& Variant<Types...>::operator=(T&& t) noexcept {
    AssignUnion::AssignHelper<Pos>(std::forward<T>(t), kInPlaceIndex<Pos>, data_);
    return *this;
}

template <std::size_t Idx, typename T>
auto&& GenericGet(T&& v) {
    return VariantHelper::GetAlt<Idx>(std::forward<T>(v));
}

// Non-member functions
template <size_t I, typename... Types>
constexpr const variant_alternative_t<I, Variant<Types...>>& Get(Variant<Types...>& v) {
    return GenericGet<I>(v);
}

template <size_t I, typename... Types>
constexpr variant_alternative_t<I, Variant<Types...>>&& Get(Variant<Types...>&& v) {
    return GenericGet<I>(v);
}

template <typename T, typename... Types>
constexpr const T& Get(Variant<Types...>& v) {
    return GenericGet<FindExactlyOne<T, Types...>::kValue>(v);
}

template <typename T, typename... Types>
constexpr T&& Get(Variant<Types...>&& v) {
    return GenericGet<FindExactlyOne<T, Types...>::kValue>(v);
}

};  // namespace task
