#include "signalforwarder.h"
#include "conversion.h"
#include "util.h"
#include <QtCore/QDebug>

namespace RubyQml {

SignalForwarder::SignalForwarder(QObject *obj, const char *signal, VALUE proc) :
    QSignalSpy(obj, signal),
    mProc(proc)
{
    setParent(obj);
    connect(obj, signal, this, SLOT(onSignal()));
    rb_gc_register_address(&mProc);
}

SignalForwarder::~SignalForwarder()
{
    rb_gc_unregister_address(&mProc);
}

void SignalForwarder::onSignal()
{
    auto args = toRuby(takeFirst());
    withGvl([&] {
        try {
            protect([&] {
                rb_funcallv(mProc, rb_intern("call"), RARRAY_LEN(args), RARRAY_PTR(args));
            });
        }
        catch (const RubyException &) {
            qWarning() << "unhandled Ruby exception in signal";
        }
    });
    clear();
}

} // namespace RubyQml
