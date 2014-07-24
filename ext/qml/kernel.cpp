#include "kernel.h"
#include "util.h"
#include <QTimer>

namespace RubyQml {

Kernel *Kernel::instance()
{
    if (!mInstance) {
        fail("QML::UninitializedError", "ruby-qml not yet initialized");
    }
    return mInstance;
}

void Kernel::init(const QList<QByteArray> &args)
{
    if (mInstance) {
        fail("QML::AlreadyInitializedErryr", "ruby-qml already initialized");
    }
    mInstance = new Kernel(args);
}

Kernel::Kernel(const QList<QByteArray> &args)
{
    mArgc = args.size();
    mArgData = args;
    mArgv = new char*[mArgc];
    std::transform(mArgData.begin(), mArgData.end(), mArgv, [](QByteArray &ba) { return ba.data(); });

    mApplication = new QApplication(mArgc, mArgv);
    mEngine = new QQmlEngine();
    mEventLoopHookTimer = new QTimer();
    mEventLoopHookTimer->setInterval(0);
    mEventLoopHookTimer->setSingleShot(false);
}

Kernel *Kernel::mInstance = nullptr;

} // namespace RubyQml
