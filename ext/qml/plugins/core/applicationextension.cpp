#include "applicationextension.h"
#include <QApplication>
#include <QTimer>
#include <QDebug>

namespace RubyQml {

ApplicationExtension::ApplicationExtension(QApplication *app) :
    mApp(app)
{
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
