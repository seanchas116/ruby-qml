#pragma once

#include <QObject>

class QGuiApplication;
class QQmlEngine;
class QQmlComponent;
class QQmlContext;

namespace RubyQml {

class ApplicationWrapper;
class ComponentWrapper;
class EngineWrapper;
class ContextWrapper;

class CorePlugin : public QObject
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.ruby-qml.RubyQml.Core")

public:
    CorePlugin(QObject *parent = 0);

public slots:
    QGuiApplication *createGuiApplication(const QVariantList &args);
    QQmlEngine *createEngine();
    QQmlComponent *createComponent(QQmlEngine *engine);
    RubyQml::ApplicationWrapper *createApplicationWrapper(QGuiApplication *app);
    RubyQml::EngineWrapper *createEngineWrapper(QQmlEngine *engine);
    RubyQml::ComponentWrapper *createComponentWrapper(QQmlComponent *component);
};


} // namespace RubyQml
