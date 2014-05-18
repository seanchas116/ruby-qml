#include "coreplugin.h"
#include "applicationwrapper.h"
#include "enginewrapper.h"
#include "componentwrapper.h"
#include "contextwrapper.h"
#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>

namespace RubyQml {

CorePlugin::CorePlugin(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<ApplicationWrapper *>();
    qRegisterMetaType<EngineWrapper *>();
    qRegisterMetaType<ComponentWrapper *>();
    qRegisterMetaType<ContextWrapper *>();
    qRegisterMetaType<QGuiApplication *>();
    qRegisterMetaType<QCoreApplication *>();
    qRegisterMetaType<QQmlEngine *>();
    qRegisterMetaType<QQmlComponent *>();
    qRegisterMetaType<QQmlContext *>();
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

QQmlContext *CorePlugin::createContext(QQmlEngine *engine)
{
    return new QQmlContext(engine);
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

ContextWrapper *CorePlugin::createContextWrapper(QQmlContext *context)
{
    return new ContextWrapper(context);
}

QQmlContext *CorePlugin::contextForObject(QObject *object)
{
    return QQmlEngine::contextForObject(object);
}

} // namespace RubyQml
