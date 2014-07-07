#include "applicationextension.h"
#include <QApplication>
#include <QTimer>
#include <QDebug>

namespace RubyQml {

ApplicationExtension::ApplicationExtension(QApplication *app) :
    mApp(app)
{
    auto timer = new QTimer(this);
    timer->setInterval(0);
    timer->setSingleShot(false);
    timer->start();
    connect(timer, &QTimer::timeout, this, &ApplicationExtension::eventsProcessed);
}

void ApplicationExtension::exec()
{
    mApp->exec();
}

void ApplicationExtension::processEvents()
{
    QCoreApplication::processEvents();
    forceDeferredDeletes();
}

void ApplicationExtension::forceDeferredDeletes()
{
    QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
}

} // namespace RubyQml
