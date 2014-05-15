#include "applicationwrapper.h"
#include <QGuiApplication>
#include <QTimer>

namespace RubyQml {

ApplicationWrapper::ApplicationWrapper(QGuiApplication *app) :
    mApp(app)
{
    auto timer = new QTimer();
    timer->setInterval(0);
    connect(timer, SIGNAL(timeout()), this, SIGNAL(eventLoopProcessed()));
}

} // namespace RubyQml
