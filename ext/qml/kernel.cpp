#include "kernel.h"
#include "util.h"

namespace RubyQml {
namespace Kernel {

namespace {

int argc;
QList<QByteArray> argData;
char **argv;

QApplication *application_;
QQmlEngine *engine_;

}

void failIfUninitialized()
{
    if (!initialized()) {
        fail("QML::UninitializedError", "ruby-qml not yet initialized");
    }
}

QApplication *application()
{
    failIfUninitialized();
    return application_;
}

QQmlEngine *engine()
{
    failIfUninitialized();
    return engine_;
}

bool initialized()
{
    return application_;
}

void init(const QList<QByteArray> &args)
{
    argc = args.size();
    argData = args;
    argv = new char*[argc];
    std::transform(argData.begin(), argData.end(), argv, [](QByteArray &ba) { return ba.data(); });

    application_ = new QApplication(argc, argv);
    engine_ = new QQmlEngine();
}

}
} // namespace RubyQml
