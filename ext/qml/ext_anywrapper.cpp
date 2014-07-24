#include "ext_anywrapper.h"
#include "rubyclass.h"

namespace RubyQml {

Ext_AnyWrapper::Ext_AnyWrapper(RubyValue self)
{
    Q_UNUSED(self);
}

RubyValue Ext_AnyWrapper::create(const QVariant &value, void (*markFunction)(const QVariant &))
{
    auto klass = wrapperRubyClass<Ext_AnyWrapper>();
    auto wrapper = klass.newInstance();
    auto ptr = klass.unwrap(wrapper);
    ptr->mValue = value;
    ptr->mMarkFunction = markFunction;
    return wrapper;
}

void Ext_AnyWrapper::defineClass()
{
    WrapperRubyClass<Ext_AnyWrapper> klass(RubyModule::fromPath("QML"), "AnyWrapper");
    Q_UNUSED(klass);
}

void Ext_AnyWrapper::gc_mark()
{
    if (mMarkFunction) {
        mMarkFunction(mValue);
    }
}

} // namespace RubyQml
