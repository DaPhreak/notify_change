#pragma once

#include <utility>
#include <type_traits>

namespace phreak {

template <class T>
class callable {
public:

    constexpr callable() noexcept = default;

    callable(callable const& s)
    : mFunction{s.mFunction?s.mFunction->clone():nullptr}
    {}

    constexpr callable(callable&& s) noexcept
    : mFunction{std::exchange(s.mFunction,nullptr)}
    {}

    template <class F,std::enable_if_t<std::is_invocable_v<F> || std::is_invocable_v<F,T>>* = nullptr>
    explicit callable(F&& function)
    : mFunction{new SimpleFunction{std::forward<F>(function)}}
    {}

    template <class O,class F,std::enable_if_t<std::is_class_v<O> && std::is_member_function_pointer_v<F> && ( std::is_invocable_v<F,O&> || std::is_invocable_v<F,O&,T> )>* = nullptr>
    callable(O& object,F function)
    : mFunction{new MemberFunction{object,std::move(function)}}
    {}

    ~callable() noexcept
    {
        delete mFunction;
    }

public:

    callable& operator = (callable const& s)
    {
        if ( &s != this ) {
            callable{s}.swap(*this);
        }
        return *this;
    }

    callable& operator = (callable&& s) noexcept
    {
        if ( &s != this ) {
            callable{std::move(s)}.swap(*this);
        }
        return *this;
    }

    void swap(callable& s) noexcept
    {
        std::swap(mFunction,s.mFunction);
    }

public:
    constexpr explicit operator bool () const noexcept { return mFunction!=nullptr; }
    
    void operator ()(T const& v) const
    {
        (*mFunction)(v);
    }

private:

    struct IFunction {
        virtual void operator ()(T const& v) const = 0;
        virtual IFunction* clone() const = 0;
        virtual ~IFunction() = default;
    };

    template <class F>
    class SimpleFunction: public IFunction {
    public:
        explicit SimpleFunction(F&& function)
        : mFunction{std::move(function)}
        {}
        explicit SimpleFunction(F const& function)
        : mFunction{function}
        {}
        void operator ()(T const& v) const override
        {
            call(v);
        }
        SimpleFunction* clone() const override
        {
            return new SimpleFunction{*this};
        }
    private:
        template <class U = T,std::enable_if_t<std::is_invocable_v<F,U>>* = nullptr>
        void call(U const& v) const
        {
            mFunction(v);
        }
        template <class U = T,std::enable_if_t<!std::is_invocable_v<F,U>>* = nullptr>
        void call(U const&) const
        {
            mFunction();
        }
        F mFunction{};
    };

    template <class O,class F>
    class MemberFunction: public IFunction {
    public:
        MemberFunction(O& object,F function)
        : mObject{object}
        , mFunction{std::move(function)}
        {}
        void operator ()(T const& v) const override
        {
            call(v);
        }
        MemberFunction* clone() const override
        {
            return new MemberFunction{*this};
        }
    private:
        template <class U = T,std::enable_if_t<std::is_invocable_v<F,O&,U>>* = nullptr>
        void call(U const& v) const
        {
            (mObject.get().*mFunction)(v);
        }
        template <class U = T,std::enable_if_t<!std::is_invocable_v<F,O&,U>>* = nullptr>
        void call(U const&) const
        {
            (mObject.get().*mFunction)();
        }
        std::reference_wrapper<O> mObject;
        F mFunction{};
    };

private:

    IFunction* mFunction{};

};

} //namespace phreak

namespace std {

template <class T>
void swap (phreak::callable<T>& lhs,phreak::callable<T>& rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace std
