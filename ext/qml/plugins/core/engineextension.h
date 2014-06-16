#pragma once

#include <QObject>

class QQmlEngine;
class QQmlContext;

namespace RubyQml {

class EngineExtension : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlContext* rootContext READ rootContext)
public:
    explicit EngineExtension(QQmlEngine *engine);
    QQmlContext *rootContext();

public slots:

private:
    QQmlEngine *mEngine;
};

} // namespace RubyQml
