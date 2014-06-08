#pragma once
#include "extbase.h"
#include "common.h"

namespace RubyQml {

class AccessClass;
class ForeignMetaObject;

namespace Ext {

class AccessSupport : public ExtBase<AccessSupport>
{
    friend class ExtBase<AccessSupport>;
public:
    AccessSupport();

    VALUE initialize(VALUE className, VALUE methodInfos, VALUE signalInfos, VALUE propertyInfos);
    VALUE emitSignal(VALUE obj, VALUE name, VALUE args);

    VALUE updateAccessObject(VALUE obj, VALUE accessObj);

private:
    void mark() {}
    static ClassBuilder buildClass();

    SP<AccessClass> mAccessClass;
    SP<ForeignMetaObject> mMetaObject;
};

} // namespace Ext
} // namespace RubyQml
