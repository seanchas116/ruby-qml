#pragma once
#include "extbase.h"
#include "common.h"

namespace RubyQml {

class AccessClass;
class ForeignMetaObject;
class QmlTypeRegisterer;

namespace Ext {

class AccessSupport : public ExtBase<AccessSupport>
{
    friend class ExtBase<AccessSupport>;
public:
    AccessSupport();
    ~AccessSupport();

    RubyValue initialize(RubyValue rubyClass, RubyValue className, RubyValue methodInfos, RubyValue signalInfos, RubyValue propertyInfos);
    RubyValue emitSignal(RubyValue obj, RubyValue name, RubyValue args);
    RubyValue registerToQml(RubyValue path, RubyValue majorVersion, RubyValue minorVersion, RubyValue name);
    RubyValue createAccessObject(RubyValue access);

    static void initClass();

private:
    void mark() {}

    RubyValue mRubyClass = Qnil;
    SP<AccessClass> mAccessClass;
    SP<ForeignMetaObject> mMetaObject;
    SP<QmlTypeRegisterer> mTypeRegisterer;
};

} // namespace Ext
} // namespace RubyQml
