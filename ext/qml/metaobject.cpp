#include "metaobject.h"
#include "conversion.h"
#include "util.h"
#include "objectpointer.h"
#include "rubyclassbase.h"
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

VALUE idListToArray(const QList<ID> &xs)
{
    VALUE ary;
    protect([&] {
        ary = rb_ary_new();
        for (ID id : xs) {
            rb_ary_push(ary, ID2SYM(id));
        }
    });
    return ary;
}

}

MetaObject::MetaObject()
{
    setMetaObject(&QObject::staticMetaObject);
}

VALUE MetaObject::className() const
{
    return rb_str_new_cstr(mMetaObject->className());
}

VALUE MetaObject::methodNames() const
{
    return idListToArray(mMethodHash.keys());
}

VALUE MetaObject::isPublic(VALUE name) const
{
    auto methods = findMethods(name);
    return toRuby(mMetaObject->method(methods.first()).access() == QMetaMethod::Public);
}
VALUE MetaObject::isProtected(VALUE name) const
{
    auto methods = findMethods(name);
    return toRuby(mMetaObject->method(methods.first()).access() == QMetaMethod::Protected);
}
VALUE MetaObject::isPrivate(VALUE name) const
{
    auto methods = findMethods(name);
    return toRuby(mMetaObject->method(methods.first()).access() == QMetaMethod::Private);
}
VALUE MetaObject::isSignal(VALUE name) const
{
    auto methods = findMethods(name);
    return toRuby(mMetaObject->method(methods.first()).methodType() == QMetaMethod::Signal);
}

class MethodInvoker
{
public:
    MethodInvoker(VALUE args, const QMetaMethod &method) :
        mArgs(args), mMethod(method) {}

    bool isArgsCompatible() const
    {
        int count = RARRAY_LEN(mArgs);
        if (mMethod.parameterCount() != count) {
            return false;
        }
        for (int i = 0; i < count; ++i) {
            auto metaType = mMethod.parameterType(i);
            if (metaType == QMetaType::QVariant) {
                return true;
            }
            auto paramCategory = metaTypeToCategory(metaType);
            auto arg = RARRAY_AREF(mArgs, i);
            auto argCategory = rubyValueCategory(arg);
            if (argCategory == TypeCategory::Invalid) {
                return false;
            }
            if (paramCategory != argCategory) {
                return false;
            }
            if (paramCategory == TypeCategory::QtObject) {
                auto paramMetaobj = QMetaType::metaObjectForType(metaType);
                if (!fromRuby<QObject *>(arg)->inherits(paramMetaobj->className())) {
                    return false;
                }
            }
        }
        return true;
    }

    VALUE invoke(QObject *obj)
    {
        std::array<QVariant, 10> argVariants;
        std::array<QGenericArgument, 10> args;
        for (int i = 0; i < mMethod.parameterCount(); ++i) {
            auto metaType = mMethod.parameterType(i);
            argVariants[i] = fromRuby(RARRAY_AREF(mArgs, i), metaType);
            args[i] = QGenericArgument(QMetaType::typeName(metaType), argVariants[i].data());
        }

        int returnType = mMethod.returnType();
        if (returnType == QMetaType::UnknownType) {
            fail("QML::MethodError", "unknown return metatype");
        }
        bool voidReturning = (returnType == QMetaType::Void);
        QVariant returnValue;
        if (!voidReturning) {
            returnValue = QVariant(returnType, QMetaType::create(returnType));
        }

        bool result;
        withoutGvl([&] {
            result = mMethod.invoke(
                        obj,
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
            auto ret = toRuby(returnValue);
            // add ownership to ObjectBase unless it has parent or is owned by QML engine
            if (isKindOf(ret, ObjectPointer::rubyClass())) {
                auto objectBase = ObjectPointer::getPointer(ret);
                auto obj = objectBase->qObject();
                if (QQmlEngine::objectOwnership(obj) == QQmlEngine::CppOwnership && !obj->parent()) {
                    objectBase->setOwnership(true);
                }
            }
            return ret;
        }
    }
private:
    VALUE mArgs;
    QMetaMethod mMethod;
};

VALUE MetaObject::invokeMethod(VALUE object, VALUE methodName, VALUE args) const
{
    auto methodIndexes = findMethods(methodName);

    protect([&] {
        args = rb_check_array_type(args);
    });
    auto obj = ObjectPointer::getPointer(object)->qObject();
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

VALUE MetaObject::connectSignal(VALUE object, VALUE signalName, VALUE proc) const
{
    auto id = idFromValue(signalName);
    auto obj = ObjectPointer::getPointer(object)->qObject();

    proc = send(proc, "to_proc");

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

VALUE MetaObject::propertyNames() const
{
    return idListToArray(mPropertyHash.keys());
}

VALUE MetaObject::getProperty(VALUE object, VALUE name) const
{
    auto metaProperty = mMetaObject->property(findProperty(name));

    auto qobj = ObjectPointer::getPointer(object)->qObject();
    QVariant result;
    withoutGvl([&] {
        result = metaProperty.read(qobj);
    });
    return toRuby(result);
}

VALUE MetaObject::setProperty(VALUE object, VALUE name, VALUE newValue) const
{
    auto metaProperty = mMetaObject->property(findProperty(name));
    if (rubyValueCategory(newValue) != metaTypeToCategory(metaProperty.userType())) {
        protect([&] {
            rb_raise(rb_path2class("QML::PropertyError"),
                     "type mismatch (%s for %s)",
                     rb_obj_classname(newValue), mMetaObject->className());
        });
    }

    auto qobj = ObjectPointer::getPointer(object)->qObject();
    auto variant = fromRuby<QVariant>(newValue);
    QVariant result;
    withoutGvl([&] {
        metaProperty.write(qobj, variant);
        result = metaProperty.read(qobj);
    });
    return toRuby(result);
}

VALUE MetaObject::notifySignal(VALUE name) const
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

QList<int> MetaObject::findMethods(VALUE name) const
{
    auto id = idFromValue(name);
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

int MetaObject::findProperty(VALUE name) const
{
    auto id = idFromValue(name);

    if (!mPropertyHash.contains(id)) {
        protect([&] {
            rb_raise(rb_path2class("QML::PropertyError"),
                     "property not found (%s in %s)",
                     rb_id2name(id), mMetaObject->className());
        });
    }
    return mPropertyHash[id];
}

VALUE MetaObject::enumerators() const
{
    VALUE hash;
    protect([&] {
        hash = rb_hash_new();
        int count = mMetaObject->enumeratorCount();
        for (int enumIndex = 0; enumIndex < count; ++enumIndex) {
            auto enumerator = mMetaObject->enumerator(enumIndex);
            for (int i = 0; i < enumerator.keyCount(); ++i) {
                rb_hash_aset(hash, toRuby(enumerator.key(i)), toRuby(enumerator.value(i)));
            }
        }
    });
    return hash;
}

VALUE MetaObject::superClass() const
{
    auto superclass = mMetaObject->superClass();
    if (!superclass) {
        return Qnil;
    }
    return fromMetaObject(superclass);
}

VALUE MetaObject::isEqual(VALUE other) const
{
    return toRuby(mMetaObject == MetaObject::getPointer(other)->mMetaObject);
}

VALUE MetaObject::hash() const
{
    return send(toRuby(reinterpret_cast<size_t>(mMetaObject)), "hash");
}

void MetaObject::setMetaObject(const QMetaObject *metaObject)
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

    mMetaObject = metaObject;
    mMethodHash = methodHash;
    mPropertyHash = propertyHash;
}

VALUE MetaObject::updateClass()
{
    return send(self(), "update_class");
}

VALUE MetaObject::fromMetaObject(const QMetaObject *metaObject)
{
    auto value = newAsRuby();
    getPointer(value)->setMetaObject(metaObject);
    return value;
}

MetaObject::ClassBuilder MetaObject::buildClass()
{
    ClassBuilder builder("QML", "MetaObject");

    builder.defineMethod<METHOD_TYPE_NAME(&MetaObject::className)>("name");

    builder.defineMethod<METHOD_TYPE_NAME(&MetaObject::methodNames)>("method_names");
    builder.defineMethod<METHOD_TYPE_NAME(&MetaObject::isPublic)>("public?");
    builder.defineMethod<METHOD_TYPE_NAME(&MetaObject::isProtected)>("protected?");
    builder.defineMethod<METHOD_TYPE_NAME(&MetaObject::isPrivate)>("private?");
    builder.defineMethod<METHOD_TYPE_NAME(&MetaObject::isSignal)>("signal?");

    builder.defineMethod<METHOD_TYPE_NAME(&MetaObject::invokeMethod)>("invoke_method");
    builder.defineMethod<METHOD_TYPE_NAME(&MetaObject::connectSignal)>("connect_signal");

    builder.defineMethod<METHOD_TYPE_NAME(&MetaObject::propertyNames)>("property_names");
    builder.defineMethod<METHOD_TYPE_NAME(&MetaObject::getProperty)>("get_property");
    builder.defineMethod<METHOD_TYPE_NAME(&MetaObject::setProperty)>("set_property");
    builder.defineMethod<METHOD_TYPE_NAME(&MetaObject::notifySignal)>("notify_signal");

    builder.defineMethod<METHOD_TYPE_NAME(&MetaObject::enumerators)>("enumerators");

    builder.defineMethod<METHOD_TYPE_NAME(&MetaObject::superClass)>("super_class");

    builder.defineMethod<METHOD_TYPE_NAME(&MetaObject::isEqual)>("==");
    builder.defineMethod<METHOD_TYPE_NAME(&MetaObject::hash)>("hash");
    builder.aliasMethod("eql?", "==");

    return builder;
}

} // namespace RubyQml
