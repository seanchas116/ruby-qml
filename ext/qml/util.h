#pragma once

#include <QMetaType>
#include <functional>
#include <type_traits>
#include <array>
#include <ruby.h>

#ifndef RARRAY_AREF
#define RARRAY_AREF(a, i)    ((RARRAY_PTR(a)[i]))
#endif

namespace RubyQml {

template <size_t... Indices>
struct IntegerSequence {};

template <size_t N, typename Seq = IntegerSequence<>, bool Last = N == 0>
struct MakeIntegerSequenceImpl
{
    using type = Seq;
};

template <size_t N, size_t... Indices>
struct MakeIntegerSequenceImpl<N, IntegerSequence<Indices...>, false> :
        MakeIntegerSequenceImpl<N - 1, IntegerSequence<N - 1, Indices...>>
{};

template <size_t N>
using MakeIntegerSequence = typename MakeIntegerSequenceImpl<N>::type;

template <typename F, typename... Args, size_t... Indices>
typename std::result_of<F(Args...)>::type
applyWithTupleImpl(const F &func, const std::tuple<Args...> &args, IntegerSequence<Indices...>)
{
    return func(std::get<Indices>(args)...);
}

template <typename F, typename... Args>
typename std::result_of<F(Args...)>::type
applyWithTuple(const F &func, const std::tuple<Args...> &args)
{
    return applyWithTupleImpl(func, args, MakeIntegerSequence<sizeof...(Args)>());
}

class RubyValue;

class RubyException
{
public:
    RubyException(int state) : mState(state) {}
    int state() const { return mState; }
private:
    int mState = 0;
};

void convertCppErrors(const std::function<void()> &doAction) noexcept;

void rescue(const std::function<void ()> &doAction, const std::function<void (RubyValue)> &handleException);
void rescueNotify(const std::function<void ()> &doAction);

// call function with GVL unlocked
void withoutGvl(const std::function<void()> &doAction);

// call function with GVL locked
void withGvl(const std::function<void()> &doAction);

void fail(const char *errorClassName, const QString &message);

template <typename ... TArgs>
void callSuper(TArgs ... args)
{
    constexpr int argc = sizeof...(args);
    std::array<VALUE, argc> argv = {{ VALUE(args)... }};
    protect([&] {
        rb_call_super(argc, argv.data());
    });
}

} // namespace RubyQml

Q_DECLARE_METATYPE(const QMetaObject*)
