#pragma once
#include "rubyvalue.h"
#include "markable.h"
#include <QObject>
#include <QMetaMethod>

namespace RubyQml {

class SignalForwarder : public QObject, public Markable
{
public:
    SignalForwarder(QObject *obj, const QMetaMethod &signal, RubyValue proc);
    ~SignalForwarder();

    int qt_metacall(QMetaObject::Call call, int id, void **args) override;
    static void deleteAll();

    void gc_mark() override;

private:
    void forwardArgs(void **args);
    void callProc(const QVariantList &list);

    QMetaMethod mSignal;
    RubyValue mProc;
    static QSet<SignalForwarder *> mInstances;
};

} // namespace RubyQml

