#include "signalforwarder.h"
#include "util.h"
#include <QtCore/QDebug>

namespace RubyQml {

SignalForwarder::SignalForwarder(QObject *obj, const QMetaMethod &signal, RubyValue proc) :
    QObject(obj),
    mSignal(signal),
    mProcRef(proc)
{
    QMetaObject::connect(obj, signal.methodIndex(), this, QObject::staticMetaObject.methodCount());
    mInstances << this;
}

SignalForwarder::~SignalForwarder()
{
    mInstances.remove(this);
}

int SignalForwarder::qt_metacall(QMetaObject::Call call, int id, void **args)
{
    id = QObject::qt_metacall(call, id, args);
    if (id < 0) {
        return id;
    }
    if (call == QMetaObject::InvokeMetaMethod) {
        if (id == 0) {
            forwardArgs(args);
        }
        --id;
    }
    return id;
}

void SignalForwarder::gc_mark()
{
    rb_gc_mark(mProcRef.value());
}

void SignalForwarder::forwardArgs(void **args)
{
    QVariantList list;
    list.reserve(mSignal.parameterCount());
    for (int i = 0; i < mSignal.parameterCount(); ++i) {
        auto type = mSignal.parameterType(i);
        if (type == QMetaType::QVariant) {
            list << *static_cast<QVariant *>(args[i + 1]);
        } else {
            list << QVariant(type, args[i + 1]);
        }
    }
    callProc(list);
}

void SignalForwarder::callProc(const QVariantList &list)
{
    if (mProcRef.hasValue()) {
        withGvl([&] {
            auto args = RubyValue::from(list);
            protect([&] {
                rb_funcallv(mProcRef.value(), rb_intern("call"), RARRAY_LEN(VALUE(args)), RARRAY_PTR(VALUE(args)));
            });
        });
    }
}

void SignalForwarder::deleteAll()
{
    auto instances = mInstances;
    for (auto instance : instances) {
        delete instance;
    }
}

QSet<SignalForwarder *> SignalForwarder::mInstances;

} // namespace RubyQml
