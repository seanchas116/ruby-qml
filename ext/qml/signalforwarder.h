#pragma once
#include <QtCore/QObject>
#include <QtCore/QMetaMethod>
#include <ruby.h>

namespace RubyQml {

class SignalForwarder : public QObject
{
public:
    SignalForwarder(QObject *obj, const QMetaMethod &signal, VALUE proc);
    ~SignalForwarder();

    int qt_metacall(QMetaObject::Call call, int id, void **args) override;

private:
    void forwardArgs(void **args);
    void callProc(const QVariantList &list);

    QMetaMethod mSignal;
    VALUE mProc;
};

} // namespace RubyQml

