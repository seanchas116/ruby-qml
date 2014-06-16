#pragma once

#include <QObject>
#include <QGuiApplication>

class QGuiApplication;

namespace RubyQml {

class ApplicationExtension : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationExtension(QApplication *app);

signals:
    void eventLoopProcessed();

public slots:
    void exec();
    void forceDeferredDeletes();

private:
    QApplication *mApp;
};

} // namespace RubyQml
