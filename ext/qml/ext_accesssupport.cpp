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

VALUE AccessSupport::initialize(VALUE rubyClass, VALUE className, VALUE methodInfos, VALUE signalInfos, VALUE propertyInfos)
{
    mRubyClass = rubyClass;
    mAccessClass = makeSP<AccessClass>(className, methodInfos, signalInfos, propertyInfos);
    mMetaObject = makeSP<ForeignMetaObject>(mAccessClass);
    return self();
}

VALUE AccessSupport::emitSignal(VALUE obj, VALUE name, VALUE args)
{
    auto accessObj = QtObjectPointer::getPointer(send(obj, "access_object"))->fetchQObject();
    auto nameId = SYM2ID(name);
    auto argVariants = fromRuby<QVariantList>(args);
    withoutGvl([&] {
        mMetaObject->emitSignal(dynamic_cast<ForeignObject *>(accessObj), nameId, argVariants);
    });
    return Qnil;
}

VALUE AccessSupport::createAccessObject(VALUE access)
{
    return QtObjectPointer::fromQObject(new AccessObject(mMetaObject, access), false);
}

VALUE AccessSupport::registerToQml(VALUE path, VALUE majorVersion, VALUE minorVersion, VALUE name)
{
    if (!mTypeRegisterer) {
        mTypeRegisterer = makeSP<QmlTypeRegisterer>(mMetaObject, [this](void *where) {
            withGvl([&] {
                auto value = send(mRubyClass, "new");
                auto obj = QtObjectPointer::fromQObject(new(where) AccessObject(mMetaObject, value), false);
                send(value, "access_object=", obj);
            });
        });
        mTypeRegisterer->registerType(fromRuby<QByteArray>(path), fromRuby<int>(majorVersion), fromRuby<int>(minorVersion), fromRuby<QByteArray>(name));
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
