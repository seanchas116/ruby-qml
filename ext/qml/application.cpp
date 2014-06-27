#include "application.h"

namespace RubyQml {
namespace Application {

namespace {

int argc;
QList<QByteArray> argData;
char **argv;

QApplication *application_;
QQmlEngine *engine_;

}

QApplication *application()
{
    return application_;
}

QQmlEngine *engine()
{
    return engine_;
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
