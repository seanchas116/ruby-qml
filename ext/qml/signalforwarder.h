#pragma once
#include "valuereference.h"
#include <QObject>
#include <QMetaMethod>

namespace RubyQml {

class SignalForwarder : public QObject
{
public:
    SignalForwarder(QObject *obj, const QMetaMethod &signal, RubyValue proc);
    ~SignalForwarder();

    int qt_metacall(QMetaObject::Call call, int id, void **args) override;
    static void deleteAll();

private:
    void forwardArgs(void **args);
    void callProc(const QVariantList &list);

    QMetaMethod mSignal;
    ValueReference mProcRef;
    static QSet<SignalForwarder *> mInstances;
};

} // namespace RubyQml

