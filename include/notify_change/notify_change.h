#pragma once

#include "callable.h"

#include <functional>

namespace phreak {

template <class T,class Condition = std::not_equal_to<T>>
class notify_change {
public:

    using value_type      = T;
    using pointer         = value_type*;
    using const_pointer	  = value_type const*;
    using reference       = value_type&;
    using const_reference = value_type const&;
    using rv_ref          = value_type&&;
    using rv_const_ref    = value_type const&&;
    using condition_type  = Condition;
    using callable_type   = callable<value_type>;

    notify_change() = delete;
    notify_change(notify_change const&) = delete;
    notify_change(notify_change&&) = delete;

    template <class F,std::enable_if_t<std::is_constructible_v<callable_type,F>>* = nullptr>
    explicit notify_change(F&& function)
    : mCallable{std::forward<F>(function)}
    {}

    template <class F,std::enable_if_t<std::is_constructible_v<callable_type,F>>* = nullptr>
    notify_change(F&& function,const_reference v)
    : mCallable{std::forward<F>(function)}
    , mValue{v}
    {}

    template <class F,std::enable_if_t<std::is_constructible_v<callable_type,F>>* = nullptr>
    notify_change(F&& function,rv_ref v)
    : mCallable{std::forward<F>(function)}
    , mValue{std::move(v)}
    {}

    template <class O,class F,std::enable_if_t<std::is_constructible_v<callable_type,O&,F>>* = nullptr>
    notify_change(O& owner,F function)
    : mCallable{owner,std::move(function)}
    {}

    template <class O,class F,std::enable_if_t<std::is_constructible_v<callable_type,O&,F>>* = nullptr>
    notify_change(O& owner,F function,const_reference v)
    : mCallable{ owner, std::move( function ) }
    , mValue{ v }
    {}

    template <class O,class F,std::enable_if_t<std::is_constructible_v<callable_type,O&,F>>* = nullptr>
    notify_change(O&owner,F function,rv_ref v)
    : mCallable{owner,std::move(function) }
    , mValue{std::move(v)}
    {}

    notify_change& operator = (notify_change const& s)
    {
        if (&s != this) {
            return *this = s.mValue;
        }
        return *this;
    }

    notify_change& operator = (notify_change&& s)
    {
        return *this = std::move(s.mValue);
    }

    notify_change& operator = (const_reference v)
    {
        const bool notify{mCallable && condition_type{}(mValue,v)};

        mValue = v;
        if (notify) {
            mCallable(mValue);
        }
        return *this;
    }

    notify_change& operator = (rv_ref v)
    {
        const bool notify{mCallable && condition_type{}(mValue,v)};

        mValue = std::move(v);
        if (notify) {
            mCallable(mValue);
        }
        return *this;
    }

public:

    [[nodiscard]] constexpr const_pointer   operator -> () const   noexcept { return &mValue; }

    [[nodiscard]] constexpr const_reference operator *  () const&  noexcept { return mValue; }
    [[nodiscard]] constexpr rv_const_ref    operator *  () const&& noexcept { return std::move(mValue); }
    [[nodiscard]] constexpr rv_ref          operator *  () &&      noexcept { return std::move(mValue); }

    [[nodiscard]] constexpr const_reference value       () const&  noexcept { return mValue; }
    [[nodiscard]] constexpr rv_const_ref    value       () const&& noexcept { return std::move(mValue); }
    [[nodiscard]] constexpr rv_ref          value       () &&      noexcept { return std::move(mValue); }

    [[nodiscard]] constexpr operator const_reference    () const&  noexcept { return mValue; }
    [[nodiscard]] constexpr operator rv_const_ref       () const&& noexcept { return std::move(mValue); }
    [[nodiscard]] constexpr operator rv_ref             () &&      noexcept { return std::move(mValue); }

private:
    value_type mValue{};
    callable_type mCallable{};
};


template <class T,class Condition>
[[nodiscard]] bool operator == (notify_change<T,Condition> const& lhs,notify_change<T,Condition> const& rhs) noexcept
{
    return *lhs == *rhs;
}

template <class T,class Condition,class U>
[[nodiscard]] bool operator == (notify_change<T,Condition> const& lhs,U const& rhs) noexcept
{
    return *lhs == rhs;
}

template <class T,class Condition,class U>
[[nodiscard]] bool operator == (U const& lhs,notify_change<T,Condition> const& rhs) noexcept
{
    return lhs == *rhs;
}

template <class T,class Condition>
[[nodiscard]] bool operator != (notify_change<T,Condition> const& lhs,notify_change<T,Condition> const& rhs) noexcept
{
    return *lhs != *rhs;
}

template <class T,class Condition,class U>
[[nodiscard]] bool operator != (notify_change<T,Condition> const& lhs,U const& rhs) noexcept
{
    return *lhs != rhs;
}

template <class T,class Condition,class U>
[[nodiscard]] bool operator != (U const& lhs,notify_change<T,Condition> const& rhs) noexcept
{
    return lhs != *rhs;
}

template <class T,class Condition>
[[nodiscard]] bool operator < (notify_change<T,Condition> const& lhs,notify_change<T,Condition> const& rhs) noexcept
{
    return *lhs < *rhs;
}

template <class T,class Condition,class U>
[[nodiscard]] bool operator < (notify_change<T,Condition> const& lhs,U const& rhs) noexcept
{
    return *lhs < rhs;
}

template <class T,class Condition,class U>
[[nodiscard]] bool operator < (U const& lhs,notify_change<T,Condition> const& rhs) noexcept
{
    return lhs < *rhs;
}

template <class T,class Condition>
[[nodiscard]] bool operator <= (notify_change<T,Condition> const& lhs,notify_change<T,Condition> const& rhs) noexcept
{
    return *lhs <= *rhs;
}

template <class T,class Condition,class U>
[[nodiscard]] bool operator <= (notify_change<T,Condition> const& lhs,U const& rhs) noexcept
{
    return *lhs <= rhs;
}

template <class T,class Condition,class U>
[[nodiscard]] bool operator <= (U const& lhs,notify_change<T,Condition> const& rhs) noexcept
{
    return lhs <= *rhs;
}

template <class T,class Condition>
[[nodiscard]] bool operator > (notify_change<T,Condition> const& lhs,notify_change<T,Condition> const& rhs) noexcept
{
    return *lhs > *rhs;
}

template <class T,class Condition,class U>
[[nodiscard]] bool operator > (notify_change<T,Condition> const& lhs,U const& rhs) noexcept
{
    return *lhs > rhs;
}

template <class T,class Condition,class U>
[[nodiscard]] bool operator > (U const& lhs,notify_change<T,Condition> const& rhs) noexcept
{
    return lhs > *rhs;
}

template <class T,class Condition>
[[nodiscard]] bool operator >= (notify_change<T,Condition> const& lhs,notify_change<T,Condition> const& rhs) noexcept
{
    return *lhs >= *rhs;
}

template <class T,class Condition,class U>
[[nodiscard]] bool operator >= (notify_change<T,Condition> const& lhs,U const& rhs) noexcept
{
    return *lhs >= rhs;
}

template <class T,class Condition,class U>
[[nodiscard]] bool operator >= (U const& lhs,notify_change<T,Condition> const& rhs) noexcept
{
    return lhs >= *rhs;
}

} //namespace phreak