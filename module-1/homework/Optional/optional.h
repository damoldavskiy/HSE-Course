#include <cstdlib>
#include <type_traits>
#include <memory>

#pragma once

namespace task
{

struct NullOpt
{
    explicit constexpr NullOpt(int)
    { }
};

constexpr NullOpt kNullOpt = NullOpt(0);

struct InPlace
{
    explicit InPlace() = default;
};

constexpr InPlace kInPlace = InPlace();

template <typename T, bool>
class OptionalDestructBase
{
public:
    constexpr OptionalDestructBase() = default;

    constexpr OptionalDestructBase(NullOpt)  // NOLINT
    { }

    template <typename... Args>
    constexpr OptionalDestructBase(InPlace, Args&&... args)  // NOLINT
        : value_(std::forward<Args>(args)...),
          exists_(true)
    { }

    template <typename U = T>
    constexpr OptionalDestructBase(U&& value)  // NOLINT
        : value_(std::forward<U>(value)),
          exists_(true)
    { }

protected:
    template <typename U = T>
    void Set(U&& value)
    {
        value_ = std::forward<U>(value);
        exists_ = true;
    }

    void Reset()
    {
        exists_ = false;
    }

    union
    {
        T value_;
        char null_;
    };

    bool exists_ = false;
};

template <typename T>
class OptionalDestructBase<T, false>
{
public:
    constexpr OptionalDestructBase()
    { }

    constexpr OptionalDestructBase(NullOpt)  // NOLINT
    { }

    template <typename... Args>
    constexpr OptionalDestructBase(InPlace, Args&&... args)  // NOLINT
        : value_(std::forward<Args>(args)...),
          exists_(true)
    { }

    template <typename U = T>
    constexpr OptionalDestructBase(U&& value)  // NOLINT
        : value_(std::forward<U>(value)),
          exists_(true)
    { }

    ~OptionalDestructBase()
    {
        if (exists_) {
            value_.~T();
        }
    }

protected:
    template <typename U = T>
    void Set(U&& value)
    {
        if (exists_) {
            value_.~T();
        }
        value_ = std::forward<U>(value);
        exists_ = true;
    }

    void Reset()
    {
        if (exists_) {
            value_.~T();
        }
        exists_ = false;
    }

    union
    {
        T value_;
        char null_;
    };

    bool exists_ = false;
};


template <typename T>
class Optional : OptionalDestructBase<T, std::is_trivially_destructible_v<T>>
{
private:
    using base = OptionalDestructBase<T, std::is_trivially_destructible<T>::value>;

public:
    using value_type = T;

    constexpr Optional() noexcept;

    template <typename U = value_type>
    constexpr Optional(U&& value);  // NOLINT

    constexpr explicit Optional(NullOpt) noexcept;

    template <typename... Args>
    constexpr explicit Optional(InPlace, Args&&... args);

    Optional& operator=(NullOpt) noexcept;

    template <typename U = T>
    Optional& operator=(U&& value);

    void Reset() noexcept;

    template <typename U>
    constexpr T ValueOr(U&& default_value) const&;

    template <typename U>
    constexpr T ValueOr(U&& default_value) &&;

    constexpr bool HasValue() const noexcept;

    constexpr explicit operator bool() const noexcept;

    constexpr std::add_pointer_t<const value_type> operator->() const;

    constexpr std::add_pointer_t<value_type> operator->();

    constexpr const value_type& operator*() const&;

    constexpr value_type& operator*() &;

    constexpr const value_type&& operator*() const&&;

    constexpr value_type&& operator*() &&;
};

template <typename T>
constexpr Optional<T>::Optional() noexcept
{ }

template <typename T>
template <typename U>
constexpr Optional<T>::Optional(U&& value)
    : base(std::forward<U>(value))
{ }

template <typename T>
constexpr Optional<T>::Optional(NullOpt) noexcept
{ }

template <typename T>
template <typename... Args>
constexpr Optional<T>::Optional(InPlace, Args&&... args)
    : base(kInPlace, std::forward<Args>(args)...)
{ }

template <typename T>
Optional<T>& Optional<T>::operator=(NullOpt) noexcept
{
    base::Reset();
    return *this;
}

template <typename T>
template <typename U>
Optional<T>& Optional<T>::operator=(U&& value)
{
    base::Set(std::forward<U>(value));
    return *this;
}

template <typename T>
void Optional<T>::Reset() noexcept
{
    base::Reset();
}

template <typename T>
template <typename U>
constexpr T Optional<T>::ValueOr(U&& default_value) const&
{
    if (base::exists_) {
        return base::value_;
    }
    return default_value;
}

template <typename T>
template <typename U>
constexpr T Optional<T>::ValueOr(U&& default_value) &&
{
    if (base::exists_) {
        return base::value_;
    }
    return default_value;
}

template <typename T>
constexpr bool Optional<T>::HasValue() const noexcept
{
    return base::exists_;
}

template <typename T>
constexpr Optional<T>::operator bool() const noexcept
{
    return HasValue();
}

template <typename T>
constexpr std::add_pointer_t<const typename Optional<T>::value_type> Optional<T>::operator->() const
{
    return base::value_;
}

template <typename T>
constexpr std::add_pointer_t<typename Optional<T>::value_type> Optional<T>::operator->()
{
    return &(base::value_);
}

template <typename T>
constexpr const typename Optional<T>::value_type& Optional<T>::operator*() const&
{
    return base::value_;
}

template <typename T>
constexpr typename Optional<T>::value_type& Optional<T>::operator*() &
{
    return base::value_;
}

template <typename T>
constexpr const typename Optional<T>::value_type&& Optional<T>::operator*() const&&
{
    return std::move(base::value_);
}

template <typename T>
constexpr typename Optional<T>::value_type&& Optional<T>::operator*() &&
{
    return std::move(base::value_);
}

}  // namespace task
