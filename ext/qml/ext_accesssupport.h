#pragma once
#include "common.h"
#include "rubyvalue.h"

namespace RubyQml {

class AccessClass;
class ForeignMetaObject;
class QmlTypeRegisterer;

namespace Ext {

class AccessSupport
{
public:
    AccessSupport(RubyValue self);
    ~AccessSupport();

    RubyValue initialize(RubyValue rubyClass, RubyValue className, RubyValue methodInfos, RubyValue signalInfos, RubyValue propertyInfos);
    RubyValue emitSignal(RubyValue obj, RubyValue name, RubyValue args);
    RubyValue registerToQml(RubyValue path, RubyValue majorVersion, RubyValue minorVersion, RubyValue name);
    RubyValue createAccessObject(RubyValue access);

    void gc_mark() {}
    static void defineClass();

private:

    const RubyValue self;
    RubyValue mRubyClass;
    SP<AccessClass> mAccessClass;
    SP<ForeignMetaObject> mMetaObject;
    SP<QmlTypeRegisterer> mTypeRegisterer;
};

} // namespace Ext
} // namespace RubyQml
