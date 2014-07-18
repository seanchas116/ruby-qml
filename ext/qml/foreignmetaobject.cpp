#include "foreignmetaobject.h"
#include "foreignclass.h"
#include "foreignobject.h"
#include <QQueue>
#include <QDebug>
#include <QQmlProperty>
#include <private/qmetaobject_p.h>

namespace RubyQml {

class ForeignMetaObject::StringPool
{
public:
    int intern(const QByteArray &str) {
        int size = mStrings.size();
        for (int i = 0; i < size; ++i) {
            if (mStrings[i] == str)
                return i;
        }
        mStrings << str;
        return size;
    }

    QVector<uint8_t> toMetaStringData()
    {
        int count = mStrings.size();
        int size = sizeof(QByteArrayData) * count;
        for (const auto &str : mStrings) {
            size += str.size() + 1;
        }
        QVector<uint8_t> data(size);
        auto arrayDatas = reinterpret_cast<QArrayData *>(data.data());
        auto stringData = reinterpret_cast<char *>(data.data() + sizeof(QByteArrayData) * count);

        int stringOffset = 0;

        for (int i = 0; i < count; ++i) {
            const auto &string = mStrings[i];
            auto size = string.size();

            // write array data
            auto arrayDataOffset = stringOffset + sizeof(QByteArrayData) * (count - i);
            new(arrayDatas + i) QArrayData { Q_REFCOUNT_INITIALIZE_STATIC, size, 0, 0, qptrdiff(arrayDataOffset) };

            // write string data
            strcpy(stringData + stringOffset, string.data());
            stringOffset += string.size() + 1;
        }

        return data;
    }

private:
    QList<QByteArray> mStrings;
};

ForeignMetaObject::ForeignMetaObject(const SP<ForeignClass> &klass, const SP<ForeignMetaObject> &superMetaObject) :
    mForeignClass(klass),
    mSuperMetaObject(superMetaObject)
{
    buildData();

    d.superdata = mSuperMetaObject ? mSuperMetaObject.get() : &QObject::staticMetaObject;
    d.stringdata = reinterpret_cast<const QByteArrayData *>(mStringData.data());
    d.data = mData.data();
    d.static_metacall = nullptr;
    d.relatedMetaObjects = nullptr;
    d.extradata = nullptr;
}

void ForeignMetaObject::emitSignal(ForeignObject *obj, std::size_t id, const QVariantList &args)
{
    auto metamethod = method(mSignalIndexHash[id] + methodOffset());
    if (metamethod.parameterCount() != args.size()) {
        qWarning() << "wrong number of signal arguments";
        return;
    }
    QVariantList argsToPass = args;
    while (argsToPass.size() < 10) {
        argsToPass << QVariant();
    }
    metamethod.invoke(obj,
                      Q_ARG(QVariant, argsToPass[0]),
                      Q_ARG(QVariant, argsToPass[1]),
                      Q_ARG(QVariant, argsToPass[2]),
                      Q_ARG(QVariant, argsToPass[3]),
                      Q_ARG(QVariant, argsToPass[4]),
                      Q_ARG(QVariant, argsToPass[5]),
                      Q_ARG(QVariant, argsToPass[6]),
                      Q_ARG(QVariant, argsToPass[7]),
                      Q_ARG(QVariant, argsToPass[8]),
                      Q_ARG(QVariant, argsToPass[9]));
}

int ForeignMetaObject::dynamicMetaCall(ForeignObject *obj, QMetaObject::Call call, int index, void **argv)
{
    if (mSuperMetaObject) {
        index = mSuperMetaObject->dynamicMetaCall(obj, call, index, argv);
    } else {
        index = obj->QObject::qt_metacall(call, index, argv);
    }

    if (index < 0) {
        return index;
    }

    switch (call) {
    case QMetaObject::InvokeMetaMethod: {
        if (index < mMethodCount) {
            if (index < mSignalCount) {
                QMetaObject::activate(obj, this, index, argv);
            } else {
                QVariantList args;
                std::transform(argv + 1, argv + 1 + mMethodArities[index], std::back_inserter(args), [](void *arg) {
                    return *reinterpret_cast<QVariant *>(arg);
                });
                auto result = mForeignClass->callMethod(obj, mMethodIds[index], args);
                if (argv[0]) {
                    *static_cast<QVariant *>(argv[0]) = result;
                }
            }
        }
        index -= mMethodCount;
        break;
    }
    case QMetaObject::ReadProperty: {
        if (index < mPropertyCount) {
            auto result = mForeignClass->getProperty(obj, mPropertyGetterIds[index]);
            *static_cast<QVariant *>(argv[0]) = result;
        }
        index -= mPropertyCount;
        break;
    }
    case QMetaObject::WriteProperty: {
        if (index < mPropertyCount) {
            auto variant = *static_cast<QVariant *>(argv[0]);
            mForeignClass->setProperty(obj, mPropertySetterIds[index], variant);
        }
        index -= mPropertyCount;
        break;
    }
    case QMetaObject::ResetProperty:
    case QMetaObject::QueryPropertyDesignable:
    case QMetaObject::QueryPropertyScriptable:
    case QMetaObject::QueryPropertyStored:
    case QMetaObject::QueryPropertyEditable:
    case QMetaObject::QueryPropertyUser: {
        index -= mPropertyCount;
        break;
    }
    case QMetaObject::RegisterPropertyMetaType: {
        if (index < mPropertyCount) {
            *static_cast<int *>(argv[0]) = -1;
        }
        index -= mPropertyCount;
        break;
    }
    default:
        break;
    }

    return index;
}

void ForeignMetaObject::buildData()
{
    auto klass = mForeignClass;
    int index = 0;
    mMethodCount = klass->methods().size();
    for (const auto &signal : klass->signalMethods()) {
        mMethodIds << signal.id;
        mMethodArities << signal.params.size();
        mSignalIndexHash[signal.id] = index++;
    }
    mSignalCount = klass->signalMethods().size();
    for (const auto &method : klass->nonSignalMethods()) {
        mMethodIds << method.id;
        mMethodArities << method.params.size();
    }
    mPropertyCount = klass->properties().size();
    for (const auto &property : klass->properties()) {
        mPropertySetterIds << property.setterId;
        mPropertyGetterIds << property.getterId;
    }

    StringPool stringPool;
    mData = writeMetaData(stringPool);
    mStringData = stringPool.toMetaStringData();
}

QVector<uint> ForeignMetaObject::writeMetaData(StringPool &stringPool)
{
    auto klass = mForeignClass;

    int methodDataSize = 0;

    for (const auto &method : klass->methods()) {
        methodDataSize += 5 + 1 + method.params.size() * 2;
    }

    QVector<uint> metaData;

    auto markIndex = [&]() {
        auto index = metaData.size();
        metaData << 0;
        return index;
    };

    auto writeCurrentPos = [&](int markedIndex) {
        metaData[markedIndex] = metaData.size();
    };

    // write header //

    metaData << 7; // revision
    metaData << stringPool.intern(klass->className()); // classname
    metaData << 0 << 0; // classinfo

    auto methods = klass->methods();
    auto signalMethods = klass->signalMethods();
    auto nonSignalMethods = klass->nonSignalMethods();

    // methods
    metaData << (methods.size());
    int methodInfoPosIndex = markIndex();

    auto properties = klass->properties();

    // properties
    metaData << properties.size();
    int propertyInfoPosIndex = markIndex();

    metaData << 0 << 0; // enums
    metaData << 0 << 0; // constructors
    metaData << 0; // flags
    metaData << signalMethods.size(); // signal count

    // write method info //

    writeCurrentPos(methodInfoPosIndex);

    QQueue<int> parameterInfoPosIndexes;

    auto addMethodInfo = [&](const ForeignClass::Method &method) {
        metaData << stringPool.intern(method.name); // name
        metaData << method.params.size(); // argc
        parameterInfoPosIndexes.enqueue(markIndex()); // parameters
        metaData << 2; // tag
        int flags = 0;
        if (method.type == ForeignClass::Method::Type::Signal) {
            flags |= AccessProtected;
            flags |= MethodSignal;
        } else {
            flags |= AccessPublic;
        }
        metaData << flags; // flags
    };

    for (const auto &signal : signalMethods) {
        addMethodInfo(signal);
    }
    for (const auto &method : nonSignalMethods) {
        addMethodInfo(method);
    }

    auto addParametersInfo = [&](const ForeignClass::Method &method) {
        writeCurrentPos(parameterInfoPosIndexes.dequeue());
        metaData << QMetaType::QVariant;
        for (int i = 0; i < method.params.size(); ++i) {
            metaData << QMetaType::QVariant;
        }
        for (const auto &param : method.params) {
            metaData << stringPool.intern(param);
        }
    };

    for (const auto &signal : signalMethods) {
        addParametersInfo(signal);
    }
    for (const auto &method : nonSignalMethods) {
        addParametersInfo(method);
    }

    // write property info //

    writeCurrentPos(propertyInfoPosIndex);

    for (const auto &property : properties) {
        metaData << stringPool.intern(property.name);
        metaData << QMetaType::QVariant;
        int flags = Notify | ResolveEditable | Stored | Scriptable | Designable;
        if (property.flags & ForeignClass::Property::Flag::Writable) {
            flags |= Writable;
        }
        if (property.flags & ForeignClass::Property::Flag::Readable) {
            flags |= Readable;
        }
        if (property.flags & ForeignClass::Property::Flag::Constant) {
            flags |= Constant;
        }
        metaData << flags;
    }

    for (const auto &property : properties) {
        int signalIndex;
        if (property.hasNotifySignal) {
            signalIndex = mSignalIndexHash[property.notifySignalId];
        } else {
            signalIndex = -1;
        }
        metaData << signalIndex;
    }

    // end of data //

    metaData << 0;

    return metaData;
}

void ForeignMetaObject::dump()
{
    qDebug() << "-- dumping ForeignClass::MetaObject" << className();
    qDebug() << "  -- methods";
    for (int i = 0; i < methodCount(); ++i) {
        auto m = method(i);
        qDebug() << "name:" << m.name();
        qDebug() << "return type:" << QMetaType::typeName(m.returnType());
        qDebug() << "param types:" << m.parameterTypes();
        qDebug() << "param names" << m.parameterNames();
        qDebug() << "method type" << m.methodType();
        qDebug() << "attributes" << m.attributes();
    }
    qDebug() << "  -- properties";
    for (int i = 0; i < propertyCount(); ++i) {
        auto p = property(i);
        qDebug() << "name:" << p.name();
        qDebug() << "notify signal:" << p.notifySignal().name();
        qDebug() << "writable" << p.isWritable();
        qDebug() << "readable" << p.isReadable();
        qDebug() << "constant" << p.isConstant();
    }
}

} // namespace RubyQml
