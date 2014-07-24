#include "ext_metaobject.h"
#include "util.h"
#include "ext_pointer.h"
#include "rubyclass.h"
#include "signalforwarder.h"
#include <QtCore/QMetaObject>
#include <QtCore/QMetaMethod>
#include <QtCore/QMetaProperty>
#include <QtCore/QMetaEnum>
#include <QtCore/QVariant>
#include <array>
#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>

namespace RubyQml {

namespace {

RubyValue idListToArray(const QList<ID> &xs)
{
    return protect([&] {
        auto ary = rb_ary_new();
        for (ID id : xs) {
            rb_ary_push(ary, ID2SYM(id));
        }
        return ary;
    });
}

}

Ext_MetaObject::Ext_MetaObject(RubyValue self) :
    self(self)
{
    setMetaObject(&QObject::staticMetaObject);
}

RubyValue Ext_MetaObject::className() const
{
    return rb_str_new_cstr(mMetaObject->className());
}

RubyValue Ext_MetaObject::methodNames() const
{
    return idListToArray(mMethodHash.keys());
}

RubyValue Ext_MetaObject::isPublic(RubyValue name) const
{
    auto methods = findMethods(name);
    return RubyValue::from(mMetaObject->method(methods.first()).access() == QMetaMethod::Public);
}
RubyValue Ext_MetaObject::isProtected(RubyValue name) const
{
    auto methods = findMethods(name);
    return RubyValue::from(mMetaObject->method(methods.first()).access() == QMetaMethod::Protected);
}
RubyValue Ext_MetaObject::isPrivate(RubyValue name) const
{
    auto methods = findMethods(name);
    return RubyValue::from(mMetaObject->method(methods.first()).access() == QMetaMethod::Private);
}
RubyValue Ext_MetaObject::isSignal(RubyValue name) const
{
    auto methods = findMethods(name);
    return RubyValue::from(mMetaObject->method(methods.first()).methodType() == QMetaMethod::Signal);
}

class MethodInvoker
{
public:
    MethodInvoker(RubyValue args, const QMetaMethod &method) :
        mArgs(args), mMethod(method) {}

    bool isArgsCompatible() const
    {
        int count = RARRAY_LEN(VALUE(mArgs));
        if (mMethod.parameterCount() != count) {
            return false;
        }
        for (int i = 0; i < count; ++i) {
            auto metaType = mMethod.parameterType(i);
            RubyValue arg = RARRAY_AREF(VALUE(mArgs), i);
            if (!arg.isConvertibleTo(metaType)) {
                return false;
            }
        }
        return true;
    }

    RubyValue invoke(QObject *obj)
    {
        std::array<QVariant, 10> argVariants;
        std::array<QGenericArgument, 10> args;
        for (int i = 0; i < mMethod.parameterCount(); ++i) {
            auto metaType = mMethod.parameterType(i);
            argVariants[i] = RubyValue(RARRAY_AREF(VALUE(mArgs), i)).toVariant(metaType);
            args[i] = QGenericArgument(QMetaType::typeName(metaType), argVariants[i].data());
        }

        int returnType = mMethod.returnType();
        if (returnType == QMetaType::UnknownType) {
            fail("QML::MethodError", "unknown return metatype");
        }
        bool voidReturning = (returnType == QMetaType::Void);
        QVariant returnValue;
        if (!voidReturning) {
            auto data = QMetaType::create(returnType);
            returnValue = QVariant(returnType, data);
            QMetaType::destroy(returnType, data);
        }

        bool result;
        withoutGvl([&] {
            result = mMethod.invoke(
                        obj,
                        Qt::DirectConnection,
                        QGenericReturnArgument(QMetaType::typeName(returnType), returnValue.data()),
                        args[0],args[1],args[2],args[3],args[4],
                        args[5],args[6],args[7],args[8],args[9]);
        });

        if (!result) {
            QString error;
            QDebug(&error) << "failed to call method" << mMethod.methodSignature();
            fail("QML::MethodError", error);
        }
        if (voidReturning) {
            return Qnil;
        } else {
            auto ret = RubyValue::from(returnValue);
            static auto objectBaseClass = RubyClass::fromPath("QML::QtObjectBase");
            if (ret.isKindOf(objectBaseClass)) {
                auto pointer = wrapperRubyClass<Ext_Pointer>().unwrap(ret.send("pointer"));
                pointer->preferManaged(true);
            }
            return ret;
        }
    }
private:
    RubyValue mArgs;
    QMetaMethod mMethod;
};

RubyValue Ext_MetaObject::invokeMethod(RubyValue object, RubyValue methodName, RubyValue args) const
{
    checkThread();

    auto methodIndexes = findMethods(methodName);

    protect([&] {
        args = rb_check_array_type(args);
    });
    auto obj = wrapperRubyClass<Ext_Pointer>().unwrap(object)->fetchQObject();
    for (int i : methodIndexes) {
        MethodInvoker invoker(args, mMetaObject->method(i));
        if (invoker.isArgsCompatible()) {
            return invoker.invoke(obj);
        }
    }
    protect([&] {
        auto to_class = rb_funcall(ID2SYM(rb_intern("class")), rb_intern("to_proc"), 0);
        auto classes = rb_funcall_with_block(args, rb_intern("map"), 0, nullptr, to_class);
        auto classes_str = rb_funcall(classes, rb_intern("to_s"), 0);

        rb_raise(rb_path2class("QML::MethodError"),
                 "method mismatch (%s with params %s in %s)",
                 mMetaObject->method(methodIndexes.first()).name().data(),
                 StringValueCStr(classes_str),
                 mMetaObject->className());
    });
    return Qnil;
}

RubyValue Ext_MetaObject::connectSignal(RubyValue object, RubyValue signalName, RubyValue proc) const
{
    auto id = signalName.toID();
    auto obj = wrapperRubyClass<Ext_Pointer>().unwrap(object)->fetchQObject();

    proc = proc.send("to_proc");

    auto methodIndexes = mMethodHash.values(id);
    std::reverse(methodIndexes.begin(), methodIndexes.end());

    for (int i : methodIndexes) {
        auto method = mMetaObject->method(i);
        if (method.methodType() != QMetaMethod::Signal) {
            continue;
        }
        new SignalForwarder(obj, method, proc);
        return Qnil;
    }
    protect([&] {
        rb_raise(rb_path2class("QML::MethodError"),
                 "signal not found (%s in %s)",
                 rb_id2name(id), mMetaObject->className());
    });
    return Qnil;
}

RubyValue Ext_MetaObject::propertyNames() const
{
    return idListToArray(mPropertyHash.keys());
}

RubyValue Ext_MetaObject::getProperty(RubyValue object, RubyValue name) const
{
    checkThread();

    auto metaProperty = mMetaObject->property(findProperty(name));

    auto qobj = wrapperRubyClass<Ext_Pointer>().unwrap(object)->fetchQObject();
    QVariant result;
    withoutGvl([&] {
        result = metaProperty.read(qobj);
    });
    return RubyValue::from(result);
}

RubyValue Ext_MetaObject::setProperty(RubyValue object, RubyValue name, RubyValue newValue) const
{
    checkThread();

    auto metaProperty = mMetaObject->property(findProperty(name));
    if (!newValue.isConvertibleTo(metaProperty.userType())) {
        protect([&] {
            rb_raise(rb_path2class("QML::PropertyError"),
                     "type mismatch (%s for %s)",
                     rb_obj_classname(newValue), metaProperty.typeName());
        });
    }

    auto qobj = wrapperRubyClass<Ext_Pointer>().unwrap(object)->fetchQObject();
    auto variant = newValue.to<QVariant>();
    QVariant result;
    withoutGvl([&] {
        metaProperty.write(qobj, variant);
        result = metaProperty.read(qobj);
    });
    return RubyValue::from(result);
}

RubyValue Ext_MetaObject::notifySignal(RubyValue name) const
{
    auto metaProperty = mMetaObject->property(findProperty(name));
    auto signal = metaProperty.notifySignal();
    if (signal.isValid()) {
        return ID2SYM(rb_intern(metaProperty.notifySignal().name()));
    }
    else {
        return Qnil;
    }
}

void Ext_MetaObject::checkThread() const
{
    if (rb_thread_current() != rb_thread_main()) {
        fail("QML::InvalidThreadError", "Qt object accessed from non-main thread");
    }
}

QList<int> Ext_MetaObject::findMethods(RubyValue name) const
{
    auto id = name.toID();
    auto methodIndexes = mMethodHash.values(id);
    if (methodIndexes.size() == 0) {
        protect([&] {
            rb_raise(rb_path2class("QML::MethodError"),
                     "method not found (%s in %s)",
                     rb_id2name(id),
                     mMetaObject->className());
        });
    }
    return methodIndexes;
}

int Ext_MetaObject::findProperty(RubyValue name) const
{
    auto id = name.toID();
    if (!mPropertyHash.contains(id)) {
        protect([&] {
            rb_raise(rb_path2class("QML::PropertyError"),
                     "property not found (%s in %s)",
                     rb_id2name(id), mMetaObject->className());
        });
    }
    return mPropertyHash[id];
}

RubyValue Ext_MetaObject::enumerators() const
{
    QHash<QByteArray, QHash<QByteArray, int>> enums;

    for (int i = mMetaObject->enumeratorOffset(); i < mMetaObject->enumeratorCount(); ++i) {
        auto metaEnum = mMetaObject->enumerator(i);
        if (metaEnum.isFlag()) {
            continue;
        }

        QHash<QByteArray, int> enumHash;
        for (int j = 0; j < metaEnum.keyCount(); ++j) {
            enumHash[metaEnum.key(j)] = metaEnum.value(j);
        }
        enums[metaEnum.name()] = enumHash;
    }

    return RubyValue::from(enums);
}

RubyValue Ext_MetaObject::superClass() const
{
    auto superclass = mMetaObject->superClass();
    if (!superclass) {
        return Qnil;
    }
    return fromMetaObject(superclass);
}

RubyValue Ext_MetaObject::isEqual(RubyValue other) const
{
    return RubyValue::from(mMetaObject == wrapperRubyClass<Ext_MetaObject>().unwrap(other)->mMetaObject);
}

RubyValue Ext_MetaObject::hash() const
{
    return RubyValue::from(reinterpret_cast<size_t>(mMetaObject)).send("hash");
}

void Ext_MetaObject::setMetaObject(const QMetaObject *metaObject)
{
    int methodCount = metaObject->methodCount() - metaObject->methodOffset();

    QMultiHash<ID, int> methodHash;
    QHash<ID, int> propertyHash;

    for (int i = 0; i < methodCount; ++i) {

        auto index = i + metaObject->methodOffset();
        auto method = metaObject->method(index);

        if (method.methodType() == QMetaMethod::Constructor) {
            continue;
        }

        methodHash.insert(rb_intern(method.name()), index);
    }

    int propertyCount = metaObject->propertyCount() - metaObject->propertyOffset();

    for (int i = 0; i < propertyCount; ++i) {
        auto index = i + metaObject->propertyOffset();
        auto property = metaObject->property(index);
        propertyHash[rb_intern(property.name())] = index;
    }

    int enumCount = metaObject->enumeratorCount() - metaObject->enumeratorOffset();
    for (int i = 0; i < enumCount; ++i) {
        auto index = i + metaObject->enumeratorOffset();
        auto metaEnum = metaObject->enumerator(index);
        auto typeName = QByteArray(metaObject->className()) + "::" + QByteArray(metaEnum.name());
        auto metaType = QMetaType::type(typeName);
        if (metaType != QMetaType::UnknownType) {
            RubyValue::addEnumeratorMetaType(metaType);
        }
    }

    mMetaObject = metaObject;
    mMethodHash = methodHash;
    mPropertyHash = propertyHash;
}

RubyValue Ext_MetaObject::fromMetaObject(const QMetaObject *metaObject)
{
    auto klass = wrapperRubyClass<Ext_MetaObject>();
    auto value = klass.newInstance();
    klass.unwrap(value)->setMetaObject(metaObject);
    return value;
}

void Ext_MetaObject::defineClass()
{
    WrapperRubyClass<Ext_MetaObject> klass(RubyModule::fromPath("QML"), "MetaObject");

    klass.defineMethod("name", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_MetaObject::className));

    klass.defineMethod("method_names", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_MetaObject::methodNames));
    klass.defineMethod("public?", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_MetaObject::isPublic));
    klass.defineMethod("protected?", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_MetaObject::isProtected));
    klass.defineMethod("private?", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_MetaObject::isPrivate));
    klass.defineMethod("signal?", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_MetaObject::isSignal));

    klass.defineMethod("invoke_method", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_MetaObject::invokeMethod));
    klass.defineMethod("connect_signal", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_MetaObject::connectSignal));

    klass.defineMethod("property_names", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_MetaObject::propertyNames));
    klass.defineMethod("get_property", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_MetaObject::getProperty));
    klass.defineMethod("set_property", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_MetaObject::setProperty));
    klass.defineMethod("notify_signal", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_MetaObject::notifySignal));

    klass.defineMethod("enumerators", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_MetaObject::enumerators));

    klass.defineMethod("super_class", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_MetaObject::superClass));

    klass.defineMethod("==", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_MetaObject::isEqual));
    klass.defineMethod("hash", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_MetaObject::hash));

    klass.aliasMethod("eql?", "==");
}

} // namespace RubyQml
