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

public slots:
    void exec();
    void processEvents();
    void forceDeferredDeletes();

private:
    QApplication *mApp;
};

} // namespace RubyQml
