#include "coreplugin.h"
#include "applicationextension.h"
#include "engineextension.h"
#include "componentextension.h"
#include "contextextension.h"
#include "rubycallbackloop.h"
#include "imageprovider.h"
#include <QApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <stdexcept>

Q_DECLARE_METATYPE(const QMetaObject*)

namespace RubyQml {

CorePlugin::CorePlugin(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<ApplicationExtension *>();
    qRegisterMetaType<EngineExtension *>();
    qRegisterMetaType<ComponentExtension *>();
    qRegisterMetaType<ContextExtension *>();
    qRegisterMetaType<RubyCallbackLoop *>();
    qRegisterMetaType<ImageProvider *>();
    qRegisterMetaType<QApplication *>();
    qRegisterMetaType<QCoreApplication *>();
    qRegisterMetaType<QQmlEngine *>();
    qRegisterMetaType<QQmlComponent *>();
    qRegisterMetaType<QQmlContext *>();

    const QMetaObject *metaObjects[] = {
        &staticQtMetaObject,
        &QApplication::staticMetaObject,
        &QQmlEngine::staticMetaObject,
        &QQmlComponent::staticMetaObject,
        &QQmlContext::staticMetaObject};
    for (auto metaobj : metaObjects) {
        mMetaObjects[metaobj->className()] = QVariant::fromValue(metaobj);
    }

    mCallbackLoop = new RubyCallbackLoop(this);
}

QApplication *CorePlugin::applicationInstance()
{
    return qApp;
}

QApplication *CorePlugin::createApplication(const QVariantList &args)
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
    auto app = new QApplication(argc, argv);
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

ApplicationExtension *CorePlugin::createApplicationExtension(QApplication *app)
{
    return new ApplicationExtension(app);
}

EngineExtension *CorePlugin::createEngineExtension(QQmlEngine *engine)
{
    return new EngineExtension(engine);
}

ComponentExtension *CorePlugin::createComponentExtension(QQmlComponent *component)
{
    return new ComponentExtension(component);
}

ContextExtension *CorePlugin::createContextExtension(QQmlContext *context)
{
    return new ContextExtension(context);
}

QQmlContext *CorePlugin::contextForObject(QObject *object)
{
    return QQmlEngine::contextForObject(object);
}

ImageProvider *CorePlugin::createImageProvider(QObject *rubyCallback)
{
    return new ImageProvider(mCallbackLoop, rubyCallback);
}

} // namespace RubyQml
