#include "applicationextension.h"
#include <QApplication>
#include <QTimer>

namespace RubyQml {

ApplicationExtension::ApplicationExtension(QApplication *app) :
    mApp(app)
{
    auto timer = new QTimer();
    timer->setInterval(0);
    connect(timer, SIGNAL(timeout()), this, SIGNAL(eventLoopProcessed()));
}

void ApplicationExtension::exec()
{
    mApp->exec();
}

} // namespace RubyQml
