#pragma once

#include <QMetaType>
#include <functional>

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
void protect(const std::function<void()> &callback);

// Regenerate Ruby exceptions that are converted into RubyException
// and convert std::exception exceptions into Ruby errors.
// Other C++ exceptions are not allowed to be thrown out of this function.
void unprotect(const std::function<void()> &callback) noexcept;

void rescue(const std::function<void ()> &doAction, const std::function<void (RubyValue)> &handleException);

// call function with GVL unlocked
void withoutGvl(const std::function<void()> &callback);

// call function with GVL locked
void withGvl(const std::function<void()> &callback);

void fail(const char *errorClassName, const QString &message);

} // namespace RubyQml

Q_DECLARE_METATYPE(const QMetaObject*)
