#pragma once

#include <ruby.h>
#include <ruby/thread.h>
#include <QtCore/QMetaType>
#include <functional>
#include <tuple>
#include <type_traits>
#include <string>

namespace RubyQml {

class RubyException
{
public:
    RubyException(int state) : mState(state) {}
    int state() const { return mState; }
private:
    int mState = 0;
};

void protect(const std::function<void()> &callback);
void unprotect(const std::function<void()> &callback);
void withoutGvl(const std::function<void()> &callback);
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

} // namespace RubyQml
