#include <QMetaObject>
#include <QMetaMethod>
#include <QMetaProperty>
#include <QMetaEnum>
#include <QSignalSpy>
#include <array>
#include <QDebug>

namespace {

class SignalForwarder : public QSignalSpy
{
    Q_OBJECT
public:
    using Callback = void (*)(const QVariant *args);
    SignalForwarder(QObject *obj, const char *signal, Callback callback) :
        QSignalSpy(obj, signal),
        mCallback(callback)
    {
        connect(obj, signal, this, SLOT(onSignal()));
    }

private slots:
    void onSignal()
    {
        mCallback(new QVariant(takeFirst()));
        clear();
    }

private:
    Callback mCallback;
};

}

extern "C" {

QVariant *qmetaobject_class_name(const QMetaObject *metaObj)
{
    return new QVariant(QString(metaObj->className()));
}

int qmetaobject_method_offset(const QMetaObject *metaObj)
{
    return metaObj->methodOffset();
}

int qmetaobject_method_count(const QMetaObject *metaObj)
{
    return metaObj->methodCount();
}

QVariant *qmetaobject_method_name(const QMetaObject *metaObj, int index)
{
    return new QVariant(QString(metaObj->method(index).name()));
}

QVariant *qmetaobject_method_parameter_names(const QMetaObject *metaObj, int index)
{
    QVariantList list;
    for (const auto &byteArray : metaObj->method(index).parameterNames()) {
        list << QString(byteArray);
    }
    return new QVariant(list);
}

QVariant *qmetaobject_method_parameter_types(const QMetaObject *metaObj, int index)
{
    auto method = metaObj->method(index);
    QVariantList list;
    for (int i = 0; i < method.parameterCount(); ++i) {
        list << method.parameterType(i);
    }
    return new QVariant(list);
}

int qmetaobject_method_is_signal(const QMetaObject *metaObj, int index)
{
    return metaObj->method(index).methodType() == QMetaMethod::Signal;
}

QVariant *qmetaobject_method_invoke(const QMetaObject *metaObj, QObject *obj, int methodIndex, const QVariant *argvVariant)
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

void qmetaobject_signal_connect(const QMetaObject *metaObj, QObject *obj, int methodIndex, void (*callback)(const QVariant *args))
{
    auto method = metaObj->method(methodIndex);

    if (method.methodType() != QMetaMethod::Signal) {
        qWarning() << "cannot connect to non-signal method" << method.methodSignature();
        return;
    }
    auto methodSig = QByteArray::number(QSIGNAL_CODE) + method.methodSignature();
    new SignalForwarder(obj, methodSig.data(), callback);
}

int qmetaobject_property_offset(const QMetaObject *metaObj)
{
    return metaObj->propertyOffset();
}

int qmetaobject_property_count(const QMetaObject *metaObj)
{
    return metaObj->propertyCount();
}

QVariant *qmetaobject_property_name(const QMetaObject *metaObj, int index)
{
    return new QVariant(QString(metaObj->property(index).name()));
}

QVariant *qmetaobject_property_get(const QMetaObject *metaObj, QObject *obj, int index)
{
    return new QVariant(metaObj->property(index).read(obj));
}

void qmetaobject_property_set(const QMetaObject *metaObj, QObject *obj, int index, const QVariant *value)
{
    metaObj->property(index).write(obj, *value);
}

int qmetaobject_property_notify_signal(const QMetaObject *metaObj, int index)
{
    return metaObj->property(index).notifySignal().methodIndex();
}

int qmetaobject_enum_offset(const QMetaObject *metaObj)
{
    return metaObj->enumeratorOffset();
}

int qmetaobject_enum_count(const QMetaObject *metaObj)
{
    return metaObj->enumeratorCount();
}

QVariant *qmetaobject_enum_get(const QMetaObject *metaObj, int enumIndex)
{
    auto enumerator = metaObj->enumerator(enumIndex);
    QVariantHash hash;
    for (int i = 0; i < enumerator.keyCount(); ++i) {
        hash[enumerator.key(i)] = enumerator.value(i);
    }
    return new QVariant(hash);
}

const QMetaObject *qmetaobject_super(const QMetaObject *metaObj)
{
    return metaObj->superClass();
}

}

#include "metaobject.moc"
