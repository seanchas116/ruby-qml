#include "metaobject.h"
#include "conversion.h"
#include "util.h"
#include "objectbase.h"
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

MetaObject::MetaObject()
{
    setMetaObject(&QObject::staticMetaObject);
}

VALUE MetaObject::className() const
{
    return rb_str_new_cstr(mMetaObject->className());
}
VALUE MetaObject::publicMethodNames() const
{
    return toRuby(mPublicMethods);
}
VALUE MetaObject::protectedMethodNames() const
{
    return toRuby(mProtectedMethods);
}
VALUE MetaObject::signalNames() const
{
    return toRuby(mSignals);
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
            auto paramCategory = metaTypeToCategory(mMethod.parameterType(i));
            auto argCategory = rubyValueCategory(RARRAY_AREF(mArgs, i));
            if (argCategory == TypeCategory::Invalid) {
                return false;
            }
            if (paramCategory != argCategory) {
                return false;
            }
        }
        return true;
    }

    VALUE invoke(VALUE object)
    {
        auto obj = fromRuby<ObjectBase *>(object)->qObject();
        std::array<QVariant, 10> argVariants;
        std::array<QGenericArgument, 10> args;
        for (int i = 0; i < mMethod.parameterCount(); ++i) {
            auto metaType = mMethod.parameterType(i);
            argVariants[i] = fromRuby(RARRAY_AREF(mArgs, i), metaType);
            args[i] = QGenericArgument(QMetaType::typeName(metaType), argVariants[i].data());
        }

        int returnType = mMethod.returnType();
        bool voidReturning = (returnType == QMetaType::Void);
        auto returnBuffer = voidReturning ? nullptr : QMetaType::create(returnType);

        auto result = mMethod.invoke(
            obj,
            QGenericReturnArgument(QMetaType::typeName(returnType), returnBuffer),
            args[0],args[1],args[2],args[3],args[4],
            args[5],args[6],args[7],args[8],args[9]);

        if (!result) {
            QString error;
            QDebug(&error) << "failed to call method" << mMethod.methodSignature();
            protect([&] {
                rb_raise(rb_path2class("QML::MethodError"), "%s", error.toUtf8().data());
            });
        }
        if (voidReturning) {
            return Qnil;
        } else {
            auto ret = toRuby(QVariant(returnType, returnBuffer));
            // add ownership to ObjectBase unless it has parent or is owned by QML engine
            if (isKindOf(ret, ObjectBase::rubyClass())) {
                auto objectBase = fromRuby<ObjectBase *>(ret);
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
    auto id = idFromValue(methodName);

    protect([&] {
        args = rb_check_array_type(args);
    });
    auto methodIndexes = mMethodHash.values(id);
    if (methodIndexes.size() == 0) {
        protect([&] {
            rb_raise(rb_path2class("QML::MethodError"),
                     "method not found (%s in %s)",
                     rb_id2name(id),
                     mMetaObject->className());
        });
    }
    for (int i : methodIndexes) {
        MethodInvoker invoker(args, mMetaObject->method(i));
        if (invoker.isArgsCompatible()) {
            return invoker.invoke(object);
        }
    }
    protect([&] {
        auto classes = rb_funcall(args, rb_intern("map"), 1, SYM2ID(rb_intern("class")));
        rb_raise(rb_path2class("QML::MethodError"),
                 "method mismatch (%s with params %s in %s)",
                 rb_id2name(id),
                 StringValueCStr(classes),
                 mMetaObject->className());
    });
    return Qnil;
}

VALUE MetaObject::connectSignal(VALUE object, VALUE signalName, VALUE proc) const
{
    auto id = idFromValue(signalName);
    auto obj = fromRuby<ObjectBase *>(object)->qObject();

    protect([&] {
        if (!rb_respond_to(proc, rb_intern("call"))) {
            rb_raise(rb_eTypeError,
                     "connecting non-callable object");
        }
    });

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
    return toRuby(mPropertyHash.keys());
}

VALUE MetaObject::getProperty(VALUE object, VALUE name) const
{
    auto metaProperty = findProperty(name);

    auto qobj = fromRuby<ObjectBase *>(object)->qObject();
    auto result = withoutGvl([&] {
        return metaProperty.read(qobj);
    });
    return toRuby(result);
}

VALUE MetaObject::setProperty(VALUE object, VALUE name, VALUE newValue) const
{
    auto metaProperty = findProperty(name);
    if (rubyValueCategory(newValue) != metaTypeToCategory(metaProperty.userType())) {
        protect([&] {
            rb_raise(rb_path2class("QML::PropertyError"),
                     "type mismatch (%s for %s)",
                     rb_obj_classname(newValue), mMetaObject->className());
        });
    }

    auto qobj = fromRuby<ObjectBase *>(object)->qObject();
    auto variant = fromRuby<QVariant>(newValue);
    auto result = withoutGvl([&] {
        metaProperty.write(qobj, variant);
        return metaProperty.read(qobj);
    });
    return toRuby(result);
}

VALUE MetaObject::notifySignal(VALUE name) const
{
    auto metaProperty = findProperty(name);
    return ID2SYM(rb_intern(metaProperty.notifySignal().name()));
}

QMetaProperty MetaObject::findProperty(VALUE name) const
{
    auto id = idFromValue(name);

    if (!mPropertyHash.contains(id)) {
        protect([&] {
            rb_raise(rb_path2class("QML::PropertyError"),
                     "property not found (%s in %s)",
                     rb_id2name(id), mMetaObject->className());
        });
    }
    return mMetaObject->property(mPropertyHash[id]);
}

VALUE MetaObject::enumerators() const
{
    return protect([&] {
        auto hash = rb_hash_new();
        int count = mMetaObject->enumeratorCount();
        for (int enumIndex = 0; enumIndex < count; ++enumIndex) {
            auto enumerator = mMetaObject->enumerator(enumIndex);
            for (int i = 0; i < enumerator.keyCount(); ++i) {
                rb_hash_aset(hash, toRuby(enumerator.key(i)), toRuby(enumerator.value(i)));
            }
        }
        return hash;
    });
}

VALUE MetaObject::superClass() const
{
    auto value = newAsRuby();
    fromRuby<MetaObject *>(value)->setMetaObject(mMetaObject->superClass());
    return value;
}

VALUE MetaObject::isEqual(VALUE other) const
{
    return toRuby(mMetaObject == fromRuby<MetaObject *>(other)->mMetaObject);
}

VALUE MetaObject::hash() const
{
    return send(toRuby(reinterpret_cast<size_t>(mMetaObject)), "hash");
}

void MetaObject::setMetaObject(const QMetaObject *metaObject)
{
    int methodCount = metaObject->methodCount() - metaObject->methodOffset();

    QMultiHash<ID, int> methodHash;
    QVector<ID> publicMethodNames, protectedMethodNames, signalNames;
    QHash<ID, int> propertyHash;

    for (int i = 0; i < methodCount; ++i) {

        auto index = i + metaObject->methodOffset() + i;
        auto method = metaObject->method(index);

        if (method.methodType() == QMetaMethod::Constructor) {
            continue;
        }

        auto name = rb_intern(method.name());

        methodHash.insert(name, index);

        if (method.access() == QMetaMethod::Public) {
            publicMethodNames << name;
        } else if (method.access() == QMetaMethod::Protected) {
            protectedMethodNames << name;
        }

        if (method.methodType() == QMetaMethod::Signal) {
            signalNames << name;
        }
    }

    int propertyCount = metaObject->methodCount() - metaObject->methodOffset();

    for (int i = 0; i < propertyCount; ++i) {
        auto index = i + metaObject->propertyOffset();
        auto property = metaObject->property(index);
        propertyHash[rb_intern(property.name())] = index;
    }

    mMetaObject = metaObject;
    mMethodHash = methodHash;
    mPublicMethods = publicMethodNames;
    mProtectedMethods = protectedMethodNames;
    mSignals = signalNames;
    mPropertyHash = propertyHash;
}

VALUE MetaObject::fromObject(QObject *obj)
{
    auto metaObj = obj->metaObject();
    auto address = toRuby(reinterpret_cast<size_t>(metaObj));
    auto include = send(metaObjectHash, "include?", address);
    if (RTEST(include)) {
        return send(metaObjectHash, "fetch", address);
    } else {
        auto metaObject = send(rubyClass(), "new");
        fromRuby<MetaObject *>(metaObject)->setMetaObject(metaObj);
        send(metaObjectHash, "[]=", address, metaObject);
        return metaObject;
    }
}

MetaObject::Definition MetaObject::createDefinition()
{
    metaObjectHash = rb_hash_new();
    rb_gc_register_mark_object(metaObjectHash);

    Definition def("QML", "MetaObject");

    def.defineMethod<METHOD_TYPE_NAME(&MetaObject::className)>("name");

    def.defineMethod<METHOD_TYPE_NAME(&MetaObject::publicMethodNames)>("public_method_names");
    def.defineMethod<METHOD_TYPE_NAME(&MetaObject::protectedMethodNames)>("protected_method_names");

    def.defineMethod<METHOD_TYPE_NAME(&MetaObject::signalNames)>("signal_names");
    def.defineMethod<METHOD_TYPE_NAME(&MetaObject::invokeMethod)>("invoke_method");
    def.defineMethod<METHOD_TYPE_NAME(&MetaObject::connectSignal)>("connect_signal");

    def.defineMethod<METHOD_TYPE_NAME(&MetaObject::propertyNames)>("property_names");
    def.defineMethod<METHOD_TYPE_NAME(&MetaObject::getProperty)>("get_property");
    def.defineMethod<METHOD_TYPE_NAME(&MetaObject::setProperty)>("set_property");
    def.defineMethod<METHOD_TYPE_NAME(&MetaObject::notifySignal)>("notify_signal");

    def.defineMethod<METHOD_TYPE_NAME(&MetaObject::enumerators)>("enumerators");

    def.defineMethod<METHOD_TYPE_NAME(&MetaObject::superClass)>("super_class");

    def.defineMethod<METHOD_TYPE_NAME(&MetaObject::isEqual)>("==");
    def.defineMethod<METHOD_TYPE_NAME(&MetaObject::hash)>("hash");
    def.aliasMethod("eql?", "==");

    return def;
}

VALUE MetaObject::metaObjectHash = Qnil;

} // namespace RubyQml
