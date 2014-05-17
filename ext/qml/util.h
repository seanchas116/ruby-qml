#pragma once

#include <ruby.h>
#include <QtCore/QMetaType>
#include <functional>

namespace RubyQml {

class RubyException
{
public:
    RubyException(int state) : mState(state) {}
    int state() const { return mState; }
private:
    int mState = 0;
};

// Convert Ruby exceptions into C++ exceptions (RubyException)
void protect(const std::function<void()> &callback);

// Regenerate Ruby exceptions that are converted into RubyException
// and convert std::exception exceptions into Ruby errors.
// Other C++ exceptions are not allowed to be thrown out of this function.
void unprotect(const std::function<void()> &callback) noexcept;

// call function with GVL unlocked
void withoutGvl(const std::function<void()> &callback);

// call function with GVL locked
void withGvl(const std::function<void()> &callback);

template <typename ... TArgs>
VALUE send(VALUE self, const char *method, TArgs && ... args)
{
    VALUE ret;
    protect([&] {
        ret = rb_funcall(self, rb_intern(method), sizeof...(args), args...);
    });
    return ret;
}

void fail(const char *errorClassName, const QString &message);

bool isKindOf(VALUE obj, VALUE klass);

// set of VALUEs that are marked in every GC
QSet<VALUE> &globalMarkValues();

} // namespace RubyQml

Q_DECLARE_METATYPE(const QMetaObject*)
