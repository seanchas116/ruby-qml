#pragma once

#include <QObject>

class QQmlEngine;
class QQmlContext;

namespace RubyQml {

class EngineExtension : public QObject
{
    Q_OBJECT
public:
    explicit EngineExtension(QQmlEngine *engine);

public slots:
    QQmlContext *rootContext();

private:
    QQmlEngine *mEngine;
};

} // namespace RubyQml
