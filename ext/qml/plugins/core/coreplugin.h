#pragma once

#include <QObject>

class QGuiApplication;
class QQmlEngine;
class QQmlComponent;
class QQmlContext;

namespace RubyQml {

class ApplicationExtension;
class ComponentExtension;
class EngineExtension;
class ContextExtension;

class CorePlugin : public QObject
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.ruby-qml.RubyQml.Core")

public:
    CorePlugin(QObject *parent = 0);

public slots:
    QGuiApplication *applicationInstance();
    QGuiApplication *createGuiApplication(const QVariantList &args);
    QQmlEngine *createEngine();
    QQmlComponent *createComponent(QQmlEngine *engine);
    QQmlContext *createContext(QQmlEngine *engine);
    RubyQml::ApplicationExtension *createApplicationExtension(QGuiApplication *app);
    RubyQml::EngineExtension *createEngineExtension(QQmlEngine *engine);
    RubyQml::ComponentExtension *createComponentExtension(QQmlComponent *component);
    RubyQml::ContextExtension *createContextExtension(QQmlContext *context);
    QQmlContext *contextForObject(QObject *object);
};


} // namespace RubyQml
