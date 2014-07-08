#pragma once

#include <QMetaType>
#include <functional>
#include <type_traits>
#include <ruby.h>

namespace RubyQml {

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
    protect([&] {
        VALUE argv[] = { args... };
        rb_call_super(sizeof...(args), argv);
    });
}

} // namespace RubyQml

Q_DECLARE_METATYPE(const QMetaObject*)
