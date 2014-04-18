#include "metaobject.h"
#include "conversion.h"
#include "util.h"
#include "objectbase.h"
#include "rubyclassbase.h"
#include <ruby.h>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaMethod>
#include <QtCore/QMetaProperty>
#include <QtCore/QMetaEnum>
#include <QtTest/QSignalSpy>
#include <array>
#include <QtCore/QDebug>

namespace RubyQml {

namespace {

class SignalForwarder : public QSignalSpy
{
    Q_OBJECT
public:
    SignalForwarder(QObject *obj, const char *signal, VALUE proc) :
        QSignalSpy(obj, signal),
        mProc(proc)
    {
        connect(obj, signal, this, SLOT(onSignal()));
    }
    ~SignalForwarder()
    {

    }

private slots:
    void onSignal()
    {
        auto args = toRuby(takeFirst());
        callWithGvl([&] {
            protectedCall([&] {
                rb_funcallv(mProc, rb_intern("call"), RARRAY_LEN(args), RARRAY_PTR(args));
            })
        });
        clear();
    }

private:
    VALUE mProc;
};

} // unnamed namespace

VALUE MetaMethod::name() const
{
    return rb_str_new_cstr(mMetaMethod.name());
}
VALUE MetaMethod::parameterNames() const
{
    std::vector<VALUE> values;
    auto names = mMetaMethod.parameterNames();
    std::transform(names.cbegin(), names.cend(), std::back_inserter(values), &qStringToRuby);
    return rb_ary_new_from_values(values.size(), values.data());
}
VALUE MetaMethod::isSignal() const
{
    return boolToRuby(mMetaMethod.methodType() == QMetaMethod::Signal);
}
VALUE MetaMethod::invoke(VALUE obj, VALUE args) const
{
    // TODO: implement
    // TODO: GVL unlocking
}
VALUE MetaMethod::connect(VALUE obj, VALUE callback) const
{
    // TODO: implement
    // TODO: GVL locking
}


VALUE MetaProperty::name() const
{
    return rb_str_new_cstr(mMetaProperty.name());
}
VALUE MetaProperty::get(VALUE object) const
{
    auto qobj = Object::fromRuby(object).qObject();
    auto result = callWithoutGvl([&] {
        return mMetaProperty.read(qobj);
    });
    return qVariantToRuby(result);
}
VALUE MetaProperty::set(VALUE object, VALUE newValue) const
{
    auto qobj = Object::fromRuby(object).qObject();
    auto variant = qVariantFromRuby(newValue);
    auto result = callWithoutGvl([&] {
        mMetaProperty.write(qobj, variant);
        return mMetaProperty.read(qobj);
    });
    return qVariantToRuby(result);
}
VALUE MetaProperty::notifySignal() const
{
    return MetaMethod::newAsRuby(mMetaProperty.notifySignal());
}


MetaObject::MetaObject(const QMetaObject *metaObject) :
    mMetaObject(metaObject)
{
    int methodCount = mMetaObject->methodCount() - mMetaObject->methodOffset();

    for (int i = 0; i < methodCount; ++i) {

        auto index = i + mMetaObject->methodOffset() + i;
        auto method = mMetaObject->method(index);

        if (method.methodType() == QMetaMethod::Constructor) {
            continue;
        }

        auto name = rb_intern(method.name());

        if (mMethodHash.contains(name)) {
            mMethodHash[name] << index;
        } else {
            mMethodHash[name] = {index};
        }

        if (method.access() == QMetaMethod::Public) {
            mPublicMethods << name;
        } else if (method.access() == QMetaMethod::Protected) {
            mProtectedMethods << name;
        }

        if (method.methodType() == QMetaMethod::Signal) {
            mSignals << name;
        }
    }

    int propertyCount = mMetaObject->methodCount() - mMetaObject->methodOffset();

    for (int i = 0; i < propertyCount; ++i) {
        auto index = i + mMetaObject->propertyOffset();
        auto property = mMetaObject->property(index);
        mPropertyHash[rb_intern(property.name())] = index;
    }
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
            auto argCategory = rubyValueCategory(*RARRAY_AREF(mArgs, i));
            if (paramCategory != argCategory) {
                return false;
            }
        }
    }
    VALUE invoke(VALUE object)
    {

    }
private:
    VALUE mArgs;
    QMetaMethod mMethod;
};

VALUE MetaObject::invokeMethod(VALUE object, VALUE methodName, VALUE args) const
{
    auto id = fromRuby<ID>(methodName);
    protectedCall([&] {
        args = rb_check_array_type(args);
    });
    for (int i : mMethodHash[id]) {
        MethodInvoker invoker(args, mMetaObject->method(i));
        if (invoker.isArgsCompatible()) {
            return invoker.invoke(object);
        }
    }
    protectedCall([&] {
        auto classes = rb_funcall(args, rb_intern("map"), 1, SYM2ID(rb_intern("class")));
        rb_raise(rb_path2class("QML::MethodError"),
                 "no matching method '%s' with params %s for Qt class '%s'",
                 StringValueCStr(methodName),
                 StringValueCStr(classes),
                 mMetaObject->className());
    });
    return Qnil;
}

VALUE MetaObject::connectSignal(VALUE object, VALUE signalName, VALUE proc) const
{
    auto id = fromRuby<ID>(signalName);
    auto obj = ObjectBase::fromRuby(object);

    protectedCall([&] {
        if (!rb_funcall(proc, rb_intern("respond_to?"), 1, ID2SYM(rb_intern("call")))) {
            rb_raise(rb_path2class("QML::SignalError"),
                     "connecting non-callable object %s", RSTRING_PTR(rb_inspect(proc)));
        }
    });

    auto methodIndexes = mMethodHash[id];
    std::reverse(methodIndexes.begin(), methodIndexes.end());

    for (int i : methodIndexes) {
        auto method = mMetaObject->method(i);
        if (method.methodType() != QMetaMethod::Signal) {
            continue;
        }
        auto methodSig = QByteArray::number(QSIGNAL_CODE) + method.methodSignature();
        new SignalForwarder(obj, methodSig.data(), callback);
        return Qnil;
    }
    protectedCall([&] {
        rb_raise(rb_path2class("QML::SignalError"),
                 "no matching signal '%s' for Qt class '%s'",
                 StringValueCStr(signalname), mMetaObject->className());
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

    auto qobj = ObjectBase::fromRuby(object).qObject();
    auto result = callWithoutGvl([&] {
        return metaProperty.read(qobj);
    });
    return fromRuby(result);
}

VALUE MetaObject::setProperty(VALUE object, VALUE name, VALUE newValue) const
{
    auto metaProperty = findProperty(name);
    if (rubyValueCategory(newValue) != metaTypeToCategory(metaProperty.userType())) {
        protectedCall([&] {
            rb_raise(rb_path2class("QML::PropertyError"),
                     "type mismatch (%s) for %s",
                     rb_obj_classname(newValue), mMetaObject->className());
        });
    }

    auto qobj = ObjectBase::fromRuby(object).qObject();
    auto variant = fromRuby<QVariant>(newValue);
    auto result = callWithoutGvl([&] {
        metaProperty.write(qobj, variant);
        return metaProperty.read(qobj);
    });
    return fromRuby(result);
}
QMetaProperty MetaObject::findProperty(VALUE name) const
{
    auto id = fromRuby<ID>(name);
    if (!mPropertyHash.contains(id)) {
        protectedCall([&] {
            rb_raise(rb_path2class("QML::PropertyError"),
                     "no matching property '%s' in %s",
                     StringValueCStr(name), mMetaObject->className());
        });
    }
    return mMetaObject->property(mPropertyHash[id]);
}
VALUE MetaObject::enumerators() const
{

}
MetaObject::Definition MetaObject::defineClass()
{
    Definition("QML", "MetaObject")
        .defineMethod<decltype(&className), &className>("name")
        .defineMethod<decltype(&publicMethodNames), &publicMethodNames>("public_method_names");

}

VALUE MetaObject::methodOffset() const
{
    return INT2FIX(mMetaObject->methodOffset());
}
VALUE MetaObject::methodCount() const
{
    return INT2FIX(mMetaObject->methodCount());
}
VALUE MetaObject::method(VALUE index) const
{
    return MetaMethod::newAsRuby(mMetaObject->method(FIX2INT(index)));
}
VALUE MetaObject::propertyOffset() const
{
    return INT2FIX(mMetaObject->propertyOffset());
}
VALUE MetaObject::propertyCount() const
{
    return INT2FIX(mMetaObject->propertyCount());
}
VALUE MetaObject::property(VALUE index) const
{
    return MetaProperty::newAsRuby(mMetaObject->property(FIX2INT(index)));
}
VALUE MetaObject::enumeratorCount() const
{
    return INT2FIX(mMetaObject->enumeratorCount());
}
VALUE MetaObject::enumerator(VALUE index) const
{
    auto metaEnum = mMetaObject->enumerator(index);
    auto hash = rb_hash_new();
    for (int i = 0; i < metaEnum.keyCount(); ++i) {
        rb_hash_aset(hash, rb_str_new_cstr(metaEnum.key(i)), INT2FIX(i));
    }
    return hash;
}
VALUE MetaObject::superClass() const
{
    return newAsRuby(mMetaObject->superClass());
}

extern "C" {

QVariant *rbqml_metaobject_class_name(const QMetaObject *metaObj)
{
    return new QVariant(QString(metaObj->className()));
}

int rbqml_metaobject_method_offset(const QMetaObject *metaObj)
{
    return metaObj->methodOffset();
}

int rbqml_metaobject_method_count(const QMetaObject *metaObj)
{
    return metaObj->methodCount();
}

QVariant *rbqml_metaobject_method_name(const QMetaObject *metaObj, int index)
{
    return new QVariant(QString(metaObj->method(index).name()));
}

QVariant *rbqml_metaobject_method_parameter_names(const QMetaObject *metaObj, int index)
{
    QVariantList list;
    auto paramNames = metaObj->method(index).parameterNames();
    std::transform(paramNames.begin(), paramNames.end(), std::back_inserter(list), &QVariant::fromValue<QString>);
    return new QVariant(list);
}

QVariant *rbqml_metaobject_method_parameter_types(const QMetaObject *metaObj, int index)
{
    auto method = metaObj->method(index);
    auto typeNames = method.parameterTypes();
    QVariantList list;
    std::transform(typeNames.begin(), typeNames.end(), std::back_inserter(list), QMetaType::type);
    return new QVariant(list);
}

int rbqml_metaobject_method_is_signal(const QMetaObject *metaObj, int index)
{
    return metaObj->method(index).methodType() == QMetaMethod::Signal;
}

QVariant *rbqml_metaobject_method_invoke(const QMetaObject *metaObj, QObject *obj, int methodIndex, const QVariant *argvVariant)
{
    auto method = metaObj->method(methodIndex);
    auto argv = argvVariant->toList();
    auto argc = argv.size();

    // convert arguments
    std::array<QGenericArgument, 10> args;

    if (argc != method.parameterCount()) {
        qWarning() << "wrong number of arguments (" << argc << "for" << method.parameterCount() << ")";
        return new QVariant();
    }

    for (int i = 0; i < argc; ++i) {
        auto metaType = method.parameterType(i);
        auto &arg = argv[i];
        if (metaType != arg.userType()) {
            qWarning() << "wrong type (" << QMetaType::typeName(arg.userType()) << "for" << QMetaType::typeName(metaType) << ")";
            return new QVariant();
        }
        args[i] = QGenericArgument(QMetaType::typeName(metaType), arg.data());
    }

    int returnType = method.returnType();
    bool voidReturning = (returnType == QMetaType::Void);
    auto returnBuffer = voidReturning ? nullptr : QMetaType::create(returnType);

    auto result = method.invoke(
        obj,
        QGenericReturnArgument(QMetaType::typeName(returnType), returnBuffer),
        args[0],args[1],args[2],args[3],args[4],
        args[5],args[6],args[7],args[8],args[9]);

    if (!result) {
        qWarning() << "failed to call method" << method.methodSignature() << "with args" << *argvVariant;
    }

    if (!voidReturning && result) {
        return new QVariant(returnType, returnBuffer);
    } else {
        return new QVariant();
    }
}

void rbqml_metaobject_signal_connect(const QMetaObject *metaObj, QObject *obj, int methodIndex, void (*callback)(const QVariant *args))
{
    auto method = metaObj->method(methodIndex);

    if (method.methodType() != QMetaMethod::Signal) {
        qWarning() << "cannot connect to non-signal method" << method.methodSignature();
        return;
    }
    auto methodSig = QByteArray::number(QSIGNAL_CODE) + method.methodSignature();
    new SignalForwarder(obj, methodSig.data(), callback);
}

int rbqml_metaobject_property_offset(const QMetaObject *metaObj)
{
    return metaObj->propertyOffset();
}

int rbqml_metaobject_property_count(const QMetaObject *metaObj)
{
    return metaObj->propertyCount();
}

QVariant *rbqml_metaobject_property_name(const QMetaObject *metaObj, int index)
{
    return new QVariant(QString(metaObj->property(index).name()));
}

QVariant *rbqml_metaobject_property_get(const QMetaObject *metaObj, QObject *obj, int index)
{
    return new QVariant(metaObj->property(index).read(obj));
}

void rbqml_metaobject_property_set(const QMetaObject *metaObj, QObject *obj, int index, const QVariant *value)
{
    metaObj->property(index).write(obj, *value);
}

int rbqml_metaobject_property_notify_signal(const QMetaObject *metaObj, int index)
{
    return metaObj->property(index).notifySignal().methodIndex();
}

int rbqml_metaobject_enum_offset(const QMetaObject *metaObj)
{
    return metaObj->enumeratorOffset();
}

int rbqml_metaobject_enum_count(const QMetaObject *metaObj)
{
    return metaObj->enumeratorCount();
}

QVariant *rbqml_metaobject_enum_get(const QMetaObject *metaObj, int enumIndex)
{
    auto enumerator = metaObj->enumerator(enumIndex);
    QVariantHash hash;
    for (int i = 0; i < enumerator.keyCount(); ++i) {
        hash[enumerator.key(i)] = enumerator.value(i);
    }
    return new QVariant(hash);
}

const QMetaObject *rbqml_metaobject_super(const QMetaObject *metaObj)
{
    return metaObj->superClass();
}

} // namespace RubyQml

#include "metaobject.moc"
