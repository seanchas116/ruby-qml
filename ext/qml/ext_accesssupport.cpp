#include "ext_accesssupport.h"
#include "ext_pointer.h"
#include "rubyclass.h"
#include "accessclass.h"
#include "accessobject.h"
#include "foreignmetaobject.h"
#include "qmltyperegisterer.h"

namespace RubyQml {
namespace Ext {

AccessWrapperFactory::AccessWrapperFactory(RubyValue self) :
    self(self)
{
}

AccessWrapperFactory::~AccessWrapperFactory()
{
}

RubyValue AccessWrapperFactory::ext_initialize(RubyValue rubyClass, RubyValue className, RubyValue methodInfos, RubyValue signalInfos, RubyValue propertyInfos)
{
    mRubyClass = rubyClass;
    mAccessClass = makeSP<AccessClass>(className, methodInfos, signalInfos, propertyInfos);
    mMetaObject = makeSP<ForeignMetaObject>(mAccessClass);
    return self;
}

RubyValue AccessWrapperFactory::ext_emitSignal(RubyValue obj, RubyValue name, RubyValue args)
{
    auto accessObj = wrapperRubyClass<Pointer>().unwrap(obj.send("access_object"))->fetchQObject();
    auto nameId = name.toID();
    auto argVariants = args.to<QVariantList>();
    withoutGvl([&] {
        mMetaObject->emitSignal(dynamic_cast<ForeignObject *>(accessObj), nameId, argVariants);
    });
    return Qnil;
}

RubyValue AccessWrapperFactory::ext_registerToQml(RubyValue path, RubyValue majorVersion, RubyValue minorVersion, RubyValue name)
{
    using namespace std::placeholders;
    if (!mTypeRegisterer) {
        mTypeRegisterer = makeSP<QmlTypeRegisterer>(mMetaObject, std::bind(&AccessWrapperFactory::newInstanceInto, this, _1));
        mTypeRegisterer->registerType(path.to<QByteArray>(), majorVersion.to<int>(), minorVersion.to<int>(), name.to<QByteArray>());
    }
    return self;
}

RubyValue AccessWrapperFactory::ext_create(RubyValue access)
{
    return RubyValue::fromQObject(create(access), false);
}

AccessWrapper *AccessWrapperFactory::create(RubyValue access)
{
    return new AccessWrapper(mMetaObject, access);
}

void AccessWrapperFactory::newInstanceInto(void *where)
{
    withGvl([&] {
        new(where) AccessWrapper(mMetaObject, mRubyClass.send("new"));
    });
}

void AccessWrapperFactory::defineClass()
{
    WrapperRubyClass<AccessWrapperFactory> klass(RubyModule::fromPath("QML"), "AccessWrapperFactory");
    klass.defineMethod(MethodAccess::Protected, "initialize", RUBYQML_MEMBER_FUNCTION_INFO(&AccessWrapperFactory::ext_initialize));
    klass.defineMethod("emit_signal", RUBYQML_MEMBER_FUNCTION_INFO(&AccessWrapperFactory::ext_emitSignal));
    klass.defineMethod("register_to_qml", RUBYQML_MEMBER_FUNCTION_INFO(&AccessWrapperFactory::ext_registerToQml));
    klass.defineMethod("create", RUBYQML_MEMBER_FUNCTION_INFO(&AccessWrapperFactory::ext_create));
}

} // namespace Ext
} // namespace RubyQml
