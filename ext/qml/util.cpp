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
    bool cxxCaught = false;
    VALUE cxxMsg = Qnil;
    try {
        callback();
    }
    catch (const RubyException &ex) {
        state = ex.state();
    }
    catch (const std::exception &ex) {
        cxxCaught = true;
        int status;
        auto classname = abi::__cxa_demangle(typeid(ex).name(), nullptr, nullptr, &status);
        cxxMsg = rb_sprintf("<%s> %s", classname, ex.what());
        free(classname);
    }
    if (state) {
        rb_jump_tag(state);
    }
    if (cxxCaught) {
        rb_exc_raise(rb_exc_new_str(rb_path2class("QML::CxxError"), cxxMsg));
    }
}

void withoutGvl(const std::function<void ()> &callback)
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
    auto result = rb_thread_call_without_gvl(f, callbackp, RUBY_UBF_IO, nullptr);
    std::unique_ptr<std::exception_ptr> exc(static_cast<std::exception_ptr *>(result));
    if (exc && *exc) {
        std::rethrow_exception(*exc);
    }
}

void withGvl(const std::function<void ()> &callback)
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
    auto result = rb_thread_call_with_gvl(f, callbackp);
    std::unique_ptr<std::exception_ptr> exc(static_cast<std::exception_ptr *>(result));
    if (exc && *exc) {
        std::rethrow_exception(*exc);
    }
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

QObject *exitHandlerObject()
{
    static auto obj = new QObject();
    return obj;
}

}
