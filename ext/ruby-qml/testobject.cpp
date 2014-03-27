#include <QObject>
#include <QTimeLine>
#include <QVariant>
#include <QDebug>

namespace RubyQml {

class TestObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER mName NOTIFY nameChanged)

public:
    explicit TestObject(QObject *parent = 0) : QObject(parent)
    {
    }

    enum Fruits
    {
        Apple = 0,
        Banana = 1,
        Orange = 2
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

    QString customType(const QMetaObject *metaobj)
    {
        return metaobj->className();
    }

signals:

    void someSignal(const QString &arg);
    void nameChanged(const QString &name);

private:

    Q_ENUMS(Fruits)
    QString mName;
};

} // RubyQml

extern "C" {

QObject *rbqml_testobject_new()
{
    return new RubyQml::TestObject();
}

const QMetaObject *rbqml_testobject_static_metaobject()
{
    return &RubyQml::TestObject::staticMetaObject;
}

}

#include "testobject.moc"
