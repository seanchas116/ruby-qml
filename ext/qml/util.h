#pragma once

#include <ruby.h>
#include <ruby/thread.h>
#include <QtCore/QMetaType>
#include <functional>
#include <tuple>
#include <type_traits>

namespace RubyQml {

class RubyException
{
public:
    RubyException(int state) : mState(state) {}
    int state() const { return mState; }
private:
    int mState = 0;
};

namespace detail {

template <
    typename TData,
    typename TSection,
    typename TCallback
>
auto sectionCall(const TSection &section, const TCallback &callback)
-> typename std::enable_if<std::is_same<decltype(callback()), void>::value, void>::type
{
    auto sectionCallback = [](TData callbackData) {
        auto callback = *(TCallback *)callbackData;
        callback();
        return TData();
    };
    section(sectionCallback, (TData)(&callback));
}

template <
    typename TData,
    typename TSection,
    typename TCallback
>
auto sectionCall(const TSection &section, const TCallback &callback)
-> typename std::enable_if<!std::is_same<decltype(callback()), void>::value, decltype(callback())>::type
{
    using Result = decltype(callback());

    auto sectionCallback = [](TData callbackData) {
        auto callback = *(TCallback *)callbackData;
        auto result = new Result(callback());
        return (TData)result;
    };
    auto resultData = section(sectionCallback, (TData)(&callback));
    auto resultPtr = (Result *)resultData;
    return Result(std::move(*resultPtr));
}

} // namespace detail

template <typename TCallback>
auto protect(const TCallback &callback) -> decltype(callback())
{
    auto section = [](VALUE (*callback)(VALUE), VALUE data) {
        int state;
        auto result = rb_protect(callback, data, &state);
        if (state) {
            throw RubyException(state);
        }
        return result;
    };
    return detail::sectionCall<VALUE>(section, callback);
}

template <typename TCallback>
auto withoutGvl(const TCallback &callback) -> decltype(callback())
{
    auto section = [](void *(*func)(void *), void *data) {
        // NOTE: is it OK to use RUBY_UBF_IO here?
        return rb_thread_call_without_gvl(func, data, RUBY_UBF_IO, nullptr);
    };
    return detail::sectionCall<void *>(section, callback);
}

template <typename TCallback>
auto withGvl(const TCallback &callback) -> decltype(callback())
{
    return detail::sectionCall<void *>(&rb_thread_call_with_gvl, callback);
}

template <typename ... TArgs>
VALUE send(VALUE self, const char *method, TArgs && ... args)
{
    return protect([&] {
        return rb_funcall(self, rb_intern(method), sizeof...(args), args...);
    });
}

void fail(const char *errorClassName, const QString &message);

} // namespace RubyQml
