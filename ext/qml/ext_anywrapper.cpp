#include "ext_anywrapper.h"
#include "rubyclass.h"

namespace RubyQml {
namespace Ext {

AnyWrapper::AnyWrapper(RubyValue self)
{
    Q_UNUSED(self);
}

RubyValue AnyWrapper::create(const QVariant &value, void (*markFunction)(const QVariant &))
{
    auto klass = wrapperRubyClass<AnyWrapper>();
    auto wrapper = klass.newInstance();
    auto ptr = klass.unwrap(wrapper);
    ptr->mValue = value;
    ptr->mMarkFunction = markFunction;
    return wrapper;
}

void AnyWrapper::defineClass()
{
    WrapperRubyClass<AnyWrapper> klass(RubyModule::fromPath("QML"), "AnyWrapper");
    Q_UNUSED(klass);
}

void AnyWrapper::gc_mark()
{
    if (mMarkFunction) {
        mMarkFunction(mValue);
    }
}

} // namespace Ext
} // namespace RubyQml
