#include "ext_accesssupport.h"
#include "ext_qtobjectpointer.h"
#include "accessclass.h"
#include "accessobject.h"

namespace RubyQml {
namespace Ext {

AccessSupport::AccessSupport()
{
}

VALUE AccessSupport::initialize(VALUE className, VALUE methodInfos, VALUE signalInfos, VALUE propertyInfos)
{
    mAccessClass = makeSP<AccessClass>(className, methodInfos, signalInfos, propertyInfos);
    mAccessClass->createMetaObject();
    return self();
}

VALUE AccessSupport::emitSignal(VALUE obj, VALUE name, VALUE args)
{
    auto accessObj = QtObjectPointer::getPointer(send(obj, "access_object"))->fetchQObject();
    auto nameId = SYM2ID(name);
    auto argVariants = fromRuby<QVariantList>(args);
    withoutGvl([&] {
        mAccessClass->emitSignal(dynamic_cast<ForeignObject *>(accessObj), nameId, argVariants);
    });
    return Qnil;
}

VALUE AccessSupport::updateAccessObject(VALUE obj, VALUE accessObj)
{
    auto accessObjectPointer = QtObjectPointer::getPointer(accessObj);
    if (!accessObjectPointer->qObject()) {
        accessObjectPointer->setQObject(new AccessObject(mAccessClass, obj), false);
    }
    return Qnil;
}

AccessSupport::ClassBuilder AccessSupport::buildClass()
{
    ClassBuilder builder("QML", "AccessSupport");
    builder.defineMethod<METHOD_TYPE_NAME(&AccessSupport::initialize)>("initialize", MethodAccess::Protected);
    builder.defineMethod<METHOD_TYPE_NAME(&AccessSupport::emitSignal)>("emit_signal");
    builder.defineMethod<METHOD_TYPE_NAME(&AccessSupport::updateAccessObject)>("update_access_object");
    return builder;
}

} // namespace Ext
} // namespace RubyQml
