#pragma once
#include <QObject>

namespace RubyQml {

class OwnershipTest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* propertyObject READ propertyObject)
public:
    explicit OwnershipTest(QObject *parent = 0);
    ~OwnershipTest();

    QObject *propertyObject() { return mPropertyObject; }

signals:
    void subObjectDeleted();
    void propertyObjectDeleted();
    void createdObjectDeleted();

public slots:
    QObject *subObject() { return mSubObject; }
    QObject *createObject();

private:
    QObject *mSubObject;
    QObject *mPropertyObject;
};

} // namespace RubyQml
