#pragma once

#include "rubyclassbase.h"
#include <QtCore/QMetaObject>
#include <QtCore/QVector>
#include <QtCore/QHash>
#include <QtCore/QObject>

namespace RubyQml {

class MetaObject : public RubyClassBase<MetaObject>
{
    friend class RubyClassBase<MetaObject>;
public:

    MetaObject(const QMetaObject *metaObject = &QObject::staticMetaObject);

    VALUE className() const;

    VALUE publicMethodNames() const;
    VALUE protectedMethodNames() const;
    VALUE signalNames() const;

    VALUE invokeMethod(VALUE object, VALUE methodName, VALUE args) const;
    VALUE connectSignal(VALUE object, VALUE signalName, VALUE proc) const;

    VALUE propertyNames() const;
    VALUE getProperty(VALUE object, VALUE name) const;
    VALUE setProperty(VALUE object, VALUE name, VALUE newValue) const;
    VALUE notifySignal(VALUE name) const;

    VALUE enumerators() const;

    VALUE superClass() const;

    VALUE isEqual(VALUE other) const;
    VALUE hash() const;

    void setMetaObject(const QMetaObject *metaObject);

private:

    static Definition createDefinition();

    QMetaProperty findProperty(VALUE name) const;

    const QMetaObject *mMetaObject = nullptr;
    QHash<ID, QVector<int>> mMethodHash;
    QVector<ID> mPublicMethods;
    QVector<ID> mProtectedMethods;
    QVector<ID> mSignals;
    QHash<ID, int> mPropertyHash;
};


}
