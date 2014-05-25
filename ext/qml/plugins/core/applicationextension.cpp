#include "applicationextension.h"
#include <QGuiApplication>
#include <QTimer>

namespace RubyQml {

ApplicationExtension::ApplicationExtension(QGuiApplication *app) :
    mApp(app)
{
    auto timer = new QTimer();
    timer->setInterval(0);
    connect(timer, SIGNAL(timeout()), this, SIGNAL(eventLoopProcessed()));
}

} // namespace RubyQml
