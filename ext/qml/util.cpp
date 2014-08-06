#include "util.h"
#include "rubyvalue.h"
#include <QString>
#include <string>
#include <memory>
#include <cxxabi.h>

#ifdef HAVE_RUBY_THREAD_H
#include <ruby/thread.h>
#else
extern "C" {
void *rb_thread_call_with_gvl(void *(*func)(void *), void *data1);
void *rb_thread_call_without_gvl(void *(*func)(void *), void *data1, rb_unblock_function_t *ubf, void *data2);
}
#endif

namespace RubyQml {

void convertCppErrors(const std::function<void ()> &doAction) noexcept
{
    bool cppErrorOccured= false;
    VALUE cppErrorClassName = Qnil;
    VALUE cppErrorMessage = Qnil;
    try {
        doAction();
    }
    catch (const std::exception &ex) {
        cppErrorOccured = true;
        int status;
        auto classname = abi::__cxa_demangle(typeid(ex).name(), nullptr, nullptr, &status);
        cppErrorClassName = rb_str_new_cstr(classname);
        free(classname);
        cppErrorMessage = rb_str_new_cstr(ex.what());
    }
    if (cppErrorOccured) {
        auto patterns = rb_funcall(rb_path2class("QML::ErrorConverter"), rb_intern("patterns"), 0);
        auto rubyClass = rb_hash_aref(patterns, cppErrorClassName);
        VALUE exc;
        if (RTEST(rubyClass)) {
            exc = rb_funcall(rubyClass, rb_intern("new"), 1, cppErrorMessage);
        } else {
            exc = rb_funcall(rb_path2class("QML::CppError"), rb_intern("new"), 2, cppErrorClassName, cppErrorMessage);
        }
        rb_exc_raise(exc);
    }
}

void rescue(const std::function<void ()> &doAction, const std::function<void (RubyValue)> &handleException)
{
    VALUE (*callback)(VALUE) = [](VALUE data) {
        (*reinterpret_cast<std::function<void ()> *>(data))();
        return Qnil;
    };
    VALUE (*rescueCallback)(VALUE, VALUE) = [](VALUE data, VALUE excObject) {
        (*reinterpret_cast<std::function<void (VALUE)>*>(data))(excObject);
        return Qnil;
    };
    rb_rescue((VALUE (*)(...))callback, (VALUE)&doAction, (VALUE (*)(...))rescueCallback, (VALUE)&handleException);
}

void rescueNotify(const std::function<void ()> &doAction)
{
    rescue(doAction, [&](RubyValue excObject) {
        rb_funcall(rb_path2class("QML::Application"), rb_intern("notify_error"), 1, excObject);
    });
}

namespace {

void changeGvl(const std::function<void ()> &doAction, bool gvl)
{
    auto actionPtr = const_cast<void *>(static_cast<const void *>(&doAction));
    auto f = [](void *data) -> void * {
        auto &doAction= *static_cast<const std::function<void ()> *>(data);
        try {
            doAction();
        } catch (...) {
            return new std::exception_ptr(std::current_exception());
        }
        return nullptr;
    };
    void *result;
    if (gvl) {
        result = rb_thread_call_with_gvl(f, actionPtr);
    } else {
        result = rb_thread_call_without_gvl(f, actionPtr, RUBY_UBF_IO, nullptr);
    }
    std::unique_ptr<std::exception_ptr> exc(static_cast<std::exception_ptr *>(result));
    if (exc && *exc) {
        std::rethrow_exception(*exc);
    }
}

}

void withoutGvl(const std::function<void ()> &doAction)
{
    changeGvl(doAction, false);
}

void withGvl(const std::function<void ()> &doAction)
{
    changeGvl(doAction, true);
}

void fail(const char *errorClassName, const QString &message)
{
    auto msg = message.toUtf8();
    rb_raise(rb_path2class(errorClassName), "%s", msg.data());
}

}
