#pragma once

#include <QObject>
#include <QGuiApplication>

class QGuiApplication;

namespace RubyQml {

class ApplicationExtension : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationExtension(QGuiApplication *app);

signals:
    void eventLoopProcessed();

public slots:
    QGuiApplication *application() { return mApp; }

private:
    QGuiApplication *mApp;
};

} // namespace RubyQml
