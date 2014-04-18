#pragma once

#include <ruby.h>
#include <ruby/thread.h>
#include <QtCore/QMetaType>
#include <functional>
#include <tuple>

Q_DECLARE_METATYPE(const QMetaObject*)

namespace RubyQml {

class RubyException
{
public:
    RubyException(int state) : mState(state) {}
    int state() const { return mState; }
private:
    int mState = 0;
};

void fail(const char *errorClassName, const QString &message)
{
    auto msg = message.toUtf8();
    protectedCall([&] {
        rb_raise(rb_path2class(errorClassName), "%s", msg);
    });
}

template <typename TSection, typename TCallback>
typename std::result_of<TCallback>::type sectionCall(const TSection &section, const TCallback &callback)
{
    using Result = typename std::result_of<TCallback>::type;
    using Tuple = decltype(std::forward_as_tuple(callback()));
    using Medium = typename std::result_of<TSection>::type; // void*, VALUE or something

    auto sectionCallback = [](Medium callbackData) {
        auto callback = *reinterpret_cast<TCallback *>(callbackData);
        auto tuple = new Tuple(std::forward_as_tuple(callback()));
        return reinterpret_cast<Medium>(tuple);
    };
    auto callbackData = reinterpret_cast<Medium>(&callback);
    auto resultData = section(sectionCallback, callbackData);

    auto tuplePtr = reinterpret_cast<Tuple *>(resultData);
    auto tuple = std::move(*tuplePtr);
    delete tuplePtr;
    return std::get<0>(tuple);
}

template <typename TCallback>
std::result_of<TCallback> protectedCall(const TCallback &callback)
{
    int state;
    auto section = [&](VALUE (*callback)(VALUE), VALUE data) {
        return rb_protect(callback, data, &state);
    };
    auto result = sectionCall(section, callback);
    if (state) {
        throw RubyException(state);
    }
    return result;
}

template <typename TCallback>
std::result_of<TCallback>::type callWithoutGvl(const TCallback &callback)
{
    auto section = [](void *(*func)(void *), void *data) {
        // NOTE: is it OK to use RUBY_UBF_IO here?
        return rb_thread_call_without_gvl(func, data, RUBY_UBF_IO, nullptr);
    };
    return sectionCall(section, callback);
}

template <typename TCallback>
std::result_of<TCallback>::type callWithGvl(const TCallback &callback)
{
    return sectionCall(&rb_thread_call_with_gvl, callback);
}

template <typename T>
inline T *getStruct(VALUE value)
{
    T *ptr;
    Data_Get_Struct(value, T, ptr);
    return ptr;
}
}
