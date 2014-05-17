#ifndef RUBYQML_APPLICATIONWRAPPER_H
#define RUBYQML_APPLICATIONWRAPPER_H

#include <QObject>
#include <QGuiApplication>

class QGuiApplication;

namespace RubyQml {

class ApplicationWrapper : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationWrapper(QGuiApplication *app);

signals:
    void eventLoopProcessed();

public slots:
    QGuiApplication *application() { return mApp; }

private:
    QGuiApplication *mApp;
};

} // namespace RubyQml

#endif // RUBYQML_APPLICATIONWRAPPER_H
