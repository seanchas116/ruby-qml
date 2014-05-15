#pragma once

#include <QObject>

namespace RubyQml {

class CorePlugin : public QObject
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.ruby-qml.RubyQml.Core")

public:
    CorePlugin(QObject *parent = 0);

public slots:
    QObject *createGuiApplication(const QVariantList &args);
    QObject *createEngine();
    QObject *createComponent(QObject *engine);
    QObject *createApplicationWrapper(QObject *app);
    QObject *createEngineWrapper(QObject *engine);
    QObject *createComponentWrapper(QObject *component);
};


} // namespace RubyQml
