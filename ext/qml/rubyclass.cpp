#include "rubyclass.h"

namespace RubyQml {

RubyModule::RubyModule(VALUE moduleValue) :
    RubyModule(RubyValue(moduleValue))
{
}

RubyModule::RubyModule(RubyValue moduleValue) :
    mValue(moduleValue)
{
    checkType();
}

RubyModule::RubyModule(const char *name)
{
    protect([&] {
        mValue = rb_define_module(name);
    });
}

RubyModule::RubyModule(const RubyModule &under, const char *name)
{
    protect([&] {
        mValue = rb_define_module_under(under.toValue(), name);
    });
}

RubyModule &RubyModule::operator=(const RubyModule &other)
{
    mValue = other.mValue;
    checkType();
    return *this;
}

RubyModule RubyModule::fromPath(const char *path)
{
    RubyValue ret;
    protect([&] {
        ret = rb_path2class(path);
    });
    return ret;
}

void RubyModule::aliasMethod(const char *newName, const char *originalName)
{
    protect([&] {
        rb_alias(mValue, rb_intern(newName), rb_intern(originalName));
    });
}

void RubyModule::checkType()
{
    if (!protect([&] { return rb_obj_is_kind_of(mValue, rb_cModule); })) {
        throw std::logic_error("expected Module value");
    }
}

RubyClass::RubyClass(RubyValue classValue) :
    RubyModule(classValue)
{
    checkType();
}

namespace {

RubyValue defineClass(const RubyModule &under, const char *name)
{
    return protect([&] {
        return rb_define_class_under(under.toValue(), name, rb_cObject);
    });
}

}

RubyClass::RubyClass(const RubyModule &under, const char *name) :
    RubyModule(defineClass(under, name))
{
}

RubyClass RubyClass::fromPath(const char *path)
{
    return RubyClass(RubyModule::fromPath(path).toValue());
}

void RubyClass::checkType()
{
    if (!toValue().isKindOf(rb_cClass)) {
        throw std::logic_error("expected Class value");
    }
}

} // namespace RubyQml
