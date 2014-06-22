#include "ext_accesssupport.h"
#include "ext_qtobjectpointer.h"
#include "rubyclass.h"
#include "accessclass.h"
#include "accessobject.h"
#include "foreignmetaobject.h"
#include "qmltyperegisterer.h"

namespace RubyQml {
namespace Ext {

AccessSupport::AccessSupport(RubyValue self) :
    self(self)
{
}

AccessSupport::~AccessSupport()
{
}

RubyValue AccessSupport::initialize(RubyValue rubyClass, RubyValue className, RubyValue methodInfos, RubyValue signalInfos, RubyValue propertyInfos)
{
    mRubyClass = rubyClass;
    mAccessClass = makeSP<AccessClass>(className, methodInfos, signalInfos, propertyInfos);
    mMetaObject = makeSP<ForeignMetaObject>(mAccessClass);
    return self;
}

RubyValue AccessSupport::emitSignal(RubyValue obj, RubyValue name, RubyValue args)
{
    auto accessObj = wrapperRubyClass<QtObjectPointer>().unwrap(obj.send("access_object"))->fetchQObject();
    auto nameId = name.toID();
    auto argVariants = args.to<QVariantList>();
    withoutGvl([&] {
        mMetaObject->emitSignal(dynamic_cast<ForeignObject *>(accessObj), nameId, argVariants);
    });
    return Qnil;
}

RubyValue AccessSupport::createAccessObject(RubyValue access)
{
    return QtObjectPointer::fromQObject(new AccessObject(mMetaObject, access), false);
}

RubyValue AccessSupport::registerToQml(RubyValue path, RubyValue majorVersion, RubyValue minorVersion, RubyValue name)
{
    if (!mTypeRegisterer) {
        mTypeRegisterer = makeSP<QmlTypeRegisterer>(mMetaObject, [this](void *where) {
            withGvl([&] {
                auto value = mRubyClass.send("new");
                auto obj = QtObjectPointer::fromQObject(new(where) AccessObject(mMetaObject, value), false);
                value.send("access_object=", obj);
            });
        });
        mTypeRegisterer->registerType(path.to<QByteArray>(), majorVersion.to<int>(), minorVersion.to<int>(), name.to<QByteArray>());
    }
    return self;
}

void AccessSupport::defineClass()
{
    WrapperRubyClass<AccessSupport> klass("QML", "AccessSupport");
    klass.defineMethod(MethodAccess::Protected, "initialize", RUBYQML_MEMBER_FUNCTION_INFO(&AccessSupport::initialize));
    klass.defineMethod("emit_signal", RUBYQML_MEMBER_FUNCTION_INFO(&AccessSupport::emitSignal));
    klass.defineMethod("create_access_object", RUBYQML_MEMBER_FUNCTION_INFO(&AccessSupport::createAccessObject));
    klass.defineMethod("register_to_qml", RUBYQML_MEMBER_FUNCTION_INFO(&AccessSupport::registerToQml));
}

} // namespace Ext
} // namespace RubyQml
