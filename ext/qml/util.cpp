#include "util.h"
#include "conversion.h"
#include <QtCore/QString>
#include <QtCore/QSet>
#include <ruby/thread.h>
#include <string>
#include <cxxabi.h>

namespace RubyQml {

void protect(const std::function<void ()> &callback)
{
    auto f = [](VALUE data) {
        auto &callback = *reinterpret_cast<const std::function<void ()> *>(data);
        callback();
        return Qnil;
    };
    int state;
    rb_protect(f, reinterpret_cast<VALUE>(&callback), &state);
    if (state) {
        throw RubyException(state);
    }
}

void unprotect(const std::function<void ()> &callback) noexcept
{
    int state = 0;
    bool cppErrorOccured= false;
    VALUE cppErrorClassName = Qnil;
    VALUE cppErrorMessage = Qnil;
    try {
        callback();
    }
    catch (const RubyException &ex) {
        state = ex.state();
    }
    catch (const std::exception &ex) {
        cppErrorOccured = true;
        int status;
        auto classname = abi::__cxa_demangle(typeid(ex).name(), nullptr, nullptr, &status);
        cppErrorClassName = rb_str_new_cstr(classname);
        free(classname);
        cppErrorMessage = rb_str_new_cstr(ex.what());
    }
    if (state) {
        rb_jump_tag(state);
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

namespace {

void withOrWithoutGvl(const std::function<void ()> &callback, bool with)
{
    auto callbackp = const_cast<void *>(static_cast<const void *>(&callback));
    auto f = [](void *data) -> void * {
        auto &callback = *static_cast<const std::function<void ()> *>(data);
        try {
            callback();
        } catch (...) {
            return new std::exception_ptr(std::current_exception());
        }
        return nullptr;
    };
    void *result;
    if (with) {
        result = rb_thread_call_with_gvl(f, callbackp);
    } else {
        result = rb_thread_call_without_gvl(f, callbackp, RUBY_UBF_IO, nullptr);
    }
    std::unique_ptr<std::exception_ptr> exc(static_cast<std::exception_ptr *>(result));
    if (exc && *exc) {
        std::rethrow_exception(*exc);
    }
}

}

void withoutGvl(const std::function<void ()> &callback)
{
    withOrWithoutGvl(callback, false);
}

void withGvl(const std::function<void ()> &callback)
{
    withOrWithoutGvl(callback, true);
}

void fail(const char *errorClassName, const QString &message)
{
    auto msg = message.toUtf8();
    protect([&] {
        rb_raise(rb_path2class(errorClassName), "%s", msg.data());
    });
}

bool isKindOf(VALUE obj, VALUE klass)
{
    VALUE result;
    protect([&] {
        result = rb_obj_is_kind_of(obj, klass);
    });
    return fromRuby<bool>(result);
}

QSet<VALUE> &globalMarkValues()
{
    static QSet<VALUE> values;
    return values;
}

}
