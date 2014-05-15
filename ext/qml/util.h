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

// convert Ruby exceptions into C++ exceptions (RubyException)
void protect(const std::function<void()> &callback);

// regenerate Ruby exceptions that are converted into RubyException
// and convert std::exception exceptions into Ruby errors
void unprotect(const std::function<void()> &callback);

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

// object that is destroyed on exit (used as exit handler)
QObject *exitHandlerObject();

} // namespace RubyQml

Q_DECLARE_METATYPE(const QMetaObject*)
