#pragma once

#include "rubyclassbase.h"
#include <QtCore/QMetaObject>

namespace RubyQml {

class MetaObject : public RubyClassBase<MetaObject>
{
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

    static Definition defineClass();

private:

    QMetaProperty findProperty(VALUE name) const;

    const QMetaObject *mMetaObject = nullptr;
    QHash<ID, QVector<int>> mMethodHash;
    QVector<ID> mPublicMethods;
    QVector<ID> mProtectedMethods;
    QVector<ID> mSignals;
    QHash<ID, int> mPropertyHash;
};


}
