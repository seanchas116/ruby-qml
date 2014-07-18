#pragma once

#include <QMetaType>
#include <functional>
#include <type_traits>
#include <ruby.h>

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


// Convert Ruby exceptions into C++ exceptions (RubyException)
void protect(const std::function<void()> &doAction);

template <typename F>
typename std::enable_if<
    !std::is_same<typename std::result_of<F()>::type, void>::value,
    typename std::result_of<F()>::type>::type
protect(const F &doAction)
{
    typename std::result_of<F()>::type ret;
    protect([&] {
        ret = doAction();
    });
    return ret;
}

// Regenerate Ruby exceptions that are converted into RubyException
// and convert std::exception exceptions into Ruby errors.
// Other C++ exceptions are not allowed to be thrown out of this function.
void unprotect(const std::function<void()> &doAction) noexcept;

void rescue(const std::function<void ()> &doAction, const std::function<void (RubyValue)> &handleException);

// call function with GVL unlocked
void withoutGvl(const std::function<void()> &doAction);

// call function with GVL locked
void withGvl(const std::function<void()> &doAction);

void fail(const char *errorClassName, const QString &message);

template <typename ... TArgs>
void callSuper(TArgs ... args)
{
    int argc = sizeof...(args);
    VALUE argv[] = { args... };
    protect([&] {
        rb_call_super(argc, argv);
    });
}

} // namespace RubyQml

Q_DECLARE_METATYPE(const QMetaObject*)
