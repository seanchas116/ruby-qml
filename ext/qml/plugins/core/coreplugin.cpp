#include "coreplugin.h"
#include "applicationwrapper.h"
#include "enginewrapper.h"
#include "componentwrapper.h"
#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlComponent>


namespace RubyQml {

namespace {

template <typename T>
T *expect(QObject *obj)
{
    auto p = qobject_cast<T *>(obj);
    if (!p) {
        throw std::invalid_argument(QString("expected %1").arg(T::staticMetaObject.className()).toStdString());
    }
    return p;
}

}

CorePlugin::CorePlugin(QObject *parent) :
    QObject(parent)
{
}

QObject *CorePlugin::createGuiApplication(const QVariantList &args)
{
    static bool created = false;
    static QList<QByteArray> staticArgs;
    static int argc;
    static char **argv;

    if (created) {
        throw std::logic_error("Application already created");
    }

    argc = args.size();
    argv = new char*[argc];

    std::transform(args.begin(), args.end(), std::back_inserter(staticArgs), [](const QVariant &arg) {
        return arg.toString().toUtf8();
    });
    std::transform(staticArgs.begin(), staticArgs.end(), argv, [](QByteArray &ba) {
        return ba.data();
    });
    auto app = new QGuiApplication(argc, argv);
    created = true;
    return app;
}

QObject *CorePlugin::createEngine()
{
    return new QQmlEngine();
}

QObject *CorePlugin::createComponent(QObject *e)
{
    return new QQmlComponent(expect<QQmlEngine>(e));
}

QObject *CorePlugin::createApplicationWrapper(QObject *app)
{
    return new ApplicationWrapper(expect<QGuiApplication>(app));
}

QObject *CorePlugin::createEngineWrapper(QObject *engine)
{
    return new EngineWrapper(expect<QQmlEngine>(engine));
}

QObject *CorePlugin::createComponentWrapper(QObject *component)
{
    return new ComponentWrapper(expect<QQmlComponent>(component));
}

} // namespace RubyQml
