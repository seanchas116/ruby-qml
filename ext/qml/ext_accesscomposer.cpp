#include "ext_accesscomposer.h"
#include "ext_objectpointer.h"
#include "accessclass.h"
#include "accessobject.h"

namespace RubyQml {
namespace Ext {

AccessComposer::AccessComposer()
{
}

VALUE AccessComposer::initialize(VALUE className, VALUE methodInfos, VALUE signalInfos, VALUE propertyInfos)
{
    mAccessClass = makeSP<AccessClass>(className, methodInfos, signalInfos, propertyInfos);
    mAccessClass->createMetaObject();
    return self();
}

VALUE AccessComposer::emitSignal(VALUE obj, VALUE name, VALUE args)
{
    auto accessObj = ObjectPointer::getPointer(send(obj, "access_object"))->fetchQObject();
    auto nameId = SYM2ID(name);
    auto argVariants = fromRuby<QVariantList>(args);
    withoutGvl([&] {
        mAccessClass->emitSignal(dynamic_cast<ForeignClass::Object *>(accessObj), nameId, argVariants);
    });
    return Qnil;
}

VALUE AccessComposer::prepareAccessObject(VALUE value)
{
    VALUE ptr;
    protect([&] {
        ptr = rb_ivar_get(value, rb_intern("@_access_object"));
    });
    if (!RTEST(ptr)) {
        ptr = ObjectPointer::newAsRuby();
        protect([&] {
            rb_ivar_set(value, rb_intern("@_access_object"), ptr);
        });
    }
    auto objectPointer = ObjectPointer::getPointer(ptr);
    if (!objectPointer->qObject()) {
        objectPointer->setQObject(new AccessObject(mAccessClass, value), false);
    }
    return ptr;
}

AccessComposer::ClassBuilder AccessComposer::buildClass()
{
    ClassBuilder builder("QML", "AccessComposer");
    builder.defineMethod<METHOD_TYPE_NAME(&AccessComposer::initialize)>("initialize", MethodAccess::Protected);
    builder.defineMethod<METHOD_TYPE_NAME(&AccessComposer::emitSignal)>("emit_signal");
    builder.defineMethod<METHOD_TYPE_NAME(&AccessComposer::prepareAccessObject)>("prepare_access_object");
    return builder;
}

} // namespace Ext
} // namespace RubyQml
