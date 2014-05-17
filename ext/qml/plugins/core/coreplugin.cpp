#include "coreplugin.h"
#include "applicationwrapper.h"
#include "enginewrapper.h"
#include "componentwrapper.h"
#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlComponent>


namespace RubyQml {

CorePlugin::CorePlugin(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<ApplicationWrapper *>();
    qRegisterMetaType<EngineWrapper *>();
    qRegisterMetaType<ComponentWrapper *>();
    qRegisterMetaType<QGuiApplication *>();
    qRegisterMetaType<QCoreApplication *>();
    qRegisterMetaType<QQmlEngine *>();
    qRegisterMetaType<QQmlComponent *>();
}

QGuiApplication *CorePlugin::createGuiApplication(const QVariantList &args)
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

QQmlEngine *CorePlugin::createEngine()
{
    return new QQmlEngine();
}

QQmlComponent *CorePlugin::createComponent(QQmlEngine *e)
{
    return new QQmlComponent(e);
}

ApplicationWrapper *CorePlugin::createApplicationWrapper(QGuiApplication *app)
{
    return new ApplicationWrapper(app);
}

EngineWrapper *CorePlugin::createEngineWrapper(QQmlEngine *engine)
{
    return new EngineWrapper(engine);
}

ComponentWrapper *CorePlugin::createComponentWrapper(QQmlComponent *component)
{
    return new ComponentWrapper(component);
}

} // namespace RubyQml
