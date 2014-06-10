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

    VALUE initialize(VALUE rubyClass, VALUE className, VALUE methodInfos, VALUE signalInfos, VALUE propertyInfos);
    VALUE emitSignal(VALUE obj, VALUE name, VALUE args);
    VALUE registerToQml(VALUE path, VALUE majorVersion, VALUE minorVersion, VALUE name);
    VALUE createAccessObject(VALUE access);

    static void initClass();

private:
    void mark() {}

    VALUE mRubyClass = Qnil;
    SP<AccessClass> mAccessClass;
    SP<ForeignMetaObject> mMetaObject;
    SP<QmlTypeRegisterer> mTypeRegisterer;
};

} // namespace Ext
} // namespace RubyQml
