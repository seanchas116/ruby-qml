#pragma once
#include "extbase.h"
#include "common.h"

namespace RubyQml {

class AccessClass;
class AccessObject;

namespace Ext {

class AccessComposer : public ExtBase<AccessComposer>
{
    friend class ExtBase<AccessComposer>;
public:
    AccessComposer();

    VALUE initialize(VALUE className, VALUE methodInfos, VALUE signalInfos, VALUE propertyInfos);
    VALUE emitSignal(VALUE obj, VALUE name, VALUE args);

    VALUE prepareAccessObject(VALUE value);

private:
    void mark() {}
    static ClassBuilder buildClass();

    SP<AccessClass> mAccessClass;
};

} // namespace Ext
} // namespace RubyQml
