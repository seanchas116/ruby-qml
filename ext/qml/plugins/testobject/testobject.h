#pragma once

#include <QObject>
#include <QVariant>
#include <QDebug>
#include <QtPlugin>

namespace RubyQml {

class TestObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER mName NOTIFY nameChanged)
    Q_PROPERTY(double someValue MEMBER mSomeValue NOTIFY someValueChanged)

public:
    explicit TestObject(QObject *parent = 0) : QObject(parent)
    {
    }

    enum Enums
    {
        Foo = 0,
        Bar = 1,
        FooBar = 2,
    };

public slots:

    int normalMethod(int x, const QString &y)
    {
        return x + y.toInt();
    }

    QVariant variantMethod(const QVariant &x, const QVariant &y)
    {
        int sum = x.toInt();
        auto list = y.toList();
        for (const auto &variant : list) {
            sum += variant.toInt();
        }
        return sum;
    }

    QString overloadedMethod(int x)
    {
        Q_UNUSED(x);
        return "first";
    }

    QString overloadedMethod(const QString &x)
    {
        Q_UNUSED(x);
        return "last";
    }

    QString overloadedMethod(const QString &x, int y)
    {
        Q_UNUSED(x)
        Q_UNUSED(y)
        return "2 params";
    }

    void emitSomeSignal(const QString &arg)
    {
        emit someSignal(arg);
    }

    QString getName() const
    {
        return mName;
    }

    void throwingMethod() const
    {
        throw std::runtime_error("test error");
    }

    const QMetaObject *metaObjectMethod() const
    {
        return &TestObject::staticMetaObject;
    }

    QObject *selfReturningMethod()
    {
        return this;
    }

signals:

    void someSignal(const QString &arg);
    void nameChanged(const QString &name);
    void someValueChanged(double value);

private:

    Q_ENUMS(Enums)
    QString mName;
    double mSomeValue = 0;
};

} // RubyQml
