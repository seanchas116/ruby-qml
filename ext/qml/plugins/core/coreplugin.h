#pragma once

#include <QObject>
#include <QVariant>

class QApplication;
class QQmlEngine;
class QQmlComponent;
class QQmlContext;

namespace RubyQml {

class ApplicationExtension;
class ComponentExtension;
class EngineExtension;
class ContextExtension;
class ImageProvider;

class CorePlugin : public QObject
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.ruby-qml.RubyQml.Core")

public:
    CorePlugin(QObject *parent = 0);

public slots:
    QVariantHash metaObjects() { return mMetaObjects; }

    QApplication *applicationInstance();

    QQmlComponent *createComponent(QQmlEngine *engine);
    QQmlContext *createContext(QQmlEngine *engine);

    RubyQml::ApplicationExtension *createApplicationExtension(QApplication *app);
    RubyQml::EngineExtension *createEngineExtension(QQmlEngine *engine);
    RubyQml::ComponentExtension *createComponentExtension(QQmlComponent *component);
    RubyQml::ContextExtension *createContextExtension(QQmlContext *context);

    QQmlContext *contextForObject(QObject *object);

    RubyQml::ImageProvider *createImageProvider(QObject *rubyCallback);

private:
    QVariantHash mMetaObjects;
};


} // namespace RubyQml
