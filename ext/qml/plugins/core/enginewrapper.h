#ifndef RUBYQML_ENGINEWRAPPER_H
#define RUBYQML_ENGINEWRAPPER_H

#include <QObject>

class QQmlEngine;
class QQmlContext;

namespace RubyQml {

class EngineWrapper : public QObject
{
    Q_OBJECT
public:
    explicit EngineWrapper(QQmlEngine *engine);

public slots:
    QQmlEngine *engine() { return mEngine; }
    QQmlContext *rootContext();

private:
    QQmlEngine *mEngine;
};

} // namespace RubyQml

#endif // RUBYQML_ENGINEWRAPPER_H
