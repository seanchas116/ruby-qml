#pragma once
#include "common.h"
#include "rubyvalue.h"

namespace RubyQml {

class AccessClass;
class ForeignMetaObject;
class QmlTypeRegisterer;
class AccessWrapper;

namespace Ext {

class AccessWrapperFactory
{
public:
    AccessWrapperFactory(RubyValue self);
    ~AccessWrapperFactory();

    RubyValue ext_initialize(RubyValue rubyClass, RubyValue className, RubyValue methodInfos, RubyValue signalInfos, RubyValue propertyInfos);
    RubyValue ext_emitSignal(RubyValue obj, RubyValue name, RubyValue args);
    RubyValue ext_registerToQml(RubyValue path, RubyValue majorVersion, RubyValue minorVersion, RubyValue name);
    RubyValue ext_create(RubyValue access);

    AccessWrapper *create(RubyValue access);

    void gc_mark() {}
    static void defineClass();

private:

    void newInstanceInto(void *where);

    const RubyValue self;
    RubyValue mRubyClass;
    SP<AccessClass> mAccessClass;
    SP<ForeignMetaObject> mMetaObject;
    SP<QmlTypeRegisterer> mTypeRegisterer;
};

} // namespace Ext
} // namespace RubyQml
