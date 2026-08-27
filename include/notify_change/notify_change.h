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
    using callable_type   = callable<value_type>;

    notify_change() = delete;
    notify_change(notify_change const&) = delete;
    notify_change(notify_change&&) = delete;

    template <class F,std::enable_if_t<std::is_constructible_v<callable<T>,F>>* = nullptr>
    explicit notify_change(F&& function)
    : mCallable{std::forward<F>(function)}
    {}

    template <class F,std::enable_if_t<std::is_constructible_v<callable<T>,F>>* = nullptr>
    notify_change(F&& function,const_reference v)
    : mCallable{std::forward<F>(function)}
    , mValue{v}
    {}

    template <class F,std::enable_if_t<std::is_constructible_v<callable<T>,F>>* = nullptr>
    notify_change(F&& function,rv_ref v)
    : mCallable{std::forward<F>(function)}
    , mValue{std::move(v)}
    {}

    template <class O,class F,std::enable_if_t<std::is_constructible_v<callable<T>,O&,F>>* = nullptr>
    notify_change(O& owner,F function)
    : mCallable{owner,std::move(function)}
    {}

    template <class O,class F,std::enable_if_t<std::is_constructible_v<callable<T>,O&,F>>* = nullptr>
    notify_change(O& owner,F function,const_reference v)
    : mCallable{ owner, std::move( function ) }
    , mValue{ v }
    {}

    template <class O,class F,std::enable_if_t<std::is_constructible_v<callable<T>,O&,F>>* = nullptr>
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
        if (Condition{}(mValue,v)) {
            mValue = v;
            notify();
        }
        return *this;
    }

    notify_change& operator = (rv_ref v)
    {
        if (Condition{}(mValue,v)) {
            mValue = std::move(v);
            notify();
        }
        return *this;
    }

public:

    [[nodiscard]] constexpr const_pointer   operator -> () const   noexcept { return &mValue; }

    [[nodiscard]] constexpr const_reference operator *  () const&  noexcept { return mValue; }
    [[nodiscard]] constexpr rv_const_ref    operator *  () const&& noexcept { return std::move(**this); }
    [[nodiscard]] constexpr rv_ref          operator *  () &&      noexcept { return std::move(**this); }

    [[nodiscard]] constexpr const_reference value       () const&  noexcept { return **this; }
    [[nodiscard]] constexpr rv_const_ref    value       () const&& noexcept { return std::move(**this); }
    [[nodiscard]] constexpr rv_ref          value       () &&      noexcept { return std::move(**this); }

    [[nodiscard]] constexpr operator const_reference    () const&  noexcept { return **this; }
    [[nodiscard]] constexpr operator rv_const_ref       () const&& noexcept { return std::move(**this); }
    [[nodiscard]] constexpr operator rv_ref             () &&      noexcept { return std::move(**this); }

private:
    void notify() const
    {
        if (mCallable) {
            mCallable(mValue);
        }
    }

    T mValue{};
    callable<T> mCallable{};
};

} //namespace phreak