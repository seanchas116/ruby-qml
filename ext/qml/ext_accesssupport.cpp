#include "ext_accesssupport.h"
#include "ext_qtobjectpointer.h"
#include "accessclass.h"
#include "accessobject.h"
#include "foreignmetaobject.h"
#include "qmltyperegisterer.h"

namespace RubyQml {
namespace Ext {

AccessSupport::AccessSupport()
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
    return self();
}

RubyValue AccessSupport::emitSignal(RubyValue obj, RubyValue name, RubyValue args)
{
    auto accessObj = QtObjectPointer::getPointer(obj.send("access_object"))->fetchQObject();
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
    return self();
}

void AccessSupport::initClass()
{
    ClassBuilder builder("QML", "AccessSupport");
    builder.defineMethod<METHOD_TYPE_NAME(&AccessSupport::initialize)>("initialize", MethodAccess::Protected);
    builder.defineMethod<METHOD_TYPE_NAME(&AccessSupport::emitSignal)>("emit_signal");
    builder.defineMethod<METHOD_TYPE_NAME(&AccessSupport::createAccessObject)>("create_access_object");
    builder.defineMethod<METHOD_TYPE_NAME(&AccessSupport::registerToQml)>("register_to_qml");
}

} // namespace Ext
} // namespace RubyQml
