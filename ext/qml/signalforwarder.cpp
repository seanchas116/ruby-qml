#include "signalforwarder.h"
#include "conversion.h"
#include "util.h"
#include "extension.h"
#include "gcprotection.h"
#include <QtCore/QDebug>

namespace RubyQml {

SignalForwarder::SignalForwarder(QObject *obj, const QMetaMethod &signal, VALUE proc) :
    QObject(obj),
    mSignal(signal),
    mProc(proc)
{
    GCProtection::add(mProc);
    QMetaObject::connect(obj, signal.methodIndex(), this, QObject::staticMetaObject.methodCount());
    connect(Extension::instance(), &QObject::destroyed, this, [&] { delete this; });
}

SignalForwarder::~SignalForwarder()
{
    GCProtection::remove(mProc);
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
    withGvl([&] {
        auto args = toRuby(list);
        protect([&] {
            rb_funcallv(mProc, rb_intern("call"), RARRAY_LEN(args), RARRAY_PTR(args));
        });
    });
}

} // namespace RubyQml
