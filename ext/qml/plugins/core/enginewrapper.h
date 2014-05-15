#ifndef RUBYQML_ENGINEWRAPPER_H
#define RUBYQML_ENGINEWRAPPER_H

#include <QObject>
#include <QQmlEngine>

class QQmlEngine;

namespace RubyQml {

class EngineWrapper : public QObject
{
    Q_OBJECT
public:
    explicit EngineWrapper(QQmlEngine *engine);

public slots:
    QObject *engine() { return mEngine; }

private:
    QQmlEngine *mEngine;
};

} // namespace RubyQml

#endif // RUBYQML_ENGINEWRAPPER_H
