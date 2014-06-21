#include "rubyclass.h"

namespace RubyQml {

RubyModule::RubyModule(RubyValue moduleValue) :
    mValue(moduleValue)
{
    checkType();
}

RubyModule::RubyModule(const QByteArray &under, const QByteArray &name)
{
    protect([&] {
        mValue = rb_define_module_under(rb_path2class(under), name);
    });
}

RubyModule &RubyModule::operator=(const RubyModule &other)
{
    mValue = other.mValue;
    checkType();
    return *this;
}

void RubyModule::aliasMethod(const char *newName, const char *originalName)
{
    protect([&] {
        rb_alias(mValue, rb_intern(newName), rb_intern(originalName));
    });
}

void RubyModule::checkType()
{
    if (!mValue.isKindOf(rb_cModule)) {
        throw std::logic_error("expected Module value");
    }
}

RubyClass::RubyClass(RubyValue classValue) :
    RubyModule(classValue)
{
    checkType();
}

static RubyValue defineClass(const QByteArray &under, const QByteArray &name)
{
    RubyValue klass;
    protect([&] {
        klass = rb_define_class_under(rb_path2class(under), name, rb_cObject);
    });
    return klass;
}

RubyClass::RubyClass(const QByteArray &under, const QByteArray &name) :
    RubyModule(defineClass(under, name))
{
}

RubyValue RubyClass::newInstance()
{
    return toValue().send("new");
}

void RubyClass::checkType()
{
    if (!toValue().isKindOf(rb_cClass)) {
        throw std::logic_error("expected Class value");
    }
}

} // namespace RubyQml
