#include "ext_kernel.h"
#include "kernel.h"
#include "rubyvalue.h"
#include "rubyclass.h"
#include <QTimer>

namespace RubyQml {
namespace Ext_Kernel {

namespace {

RubyValue application(RubyValue self)
{
    Q_UNUSED(self);
    return RubyValue::from(Kernel::instance()->application());
}

RubyValue engine(RubyValue self)
{
    Q_UNUSED(self);
    return RubyValue::from(Kernel::instance()->engine());
}

RubyValue eventLoopHookTimer(RubyValue self)
{
    Q_UNUSED(self);
    return RubyValue::from(Kernel::instance()->eventLoopHookTimer());
}

RubyValue setEventLoopHookEnabledLater(RubyValue self, RubyValue enabled)
{
    Q_UNUSED(self);
    auto timer = Kernel::instance()->eventLoopHookTimer();
    if (enabled) {
        timer->metaObject()->invokeMethod(timer, "start", Qt::QueuedConnection);
    } else {
        timer->metaObject()->invokeMethod(timer, "stop", Qt::QueuedConnection);
    }
    return enabled;
}

RubyValue applicationMetaObject()
{
    return RubyValue::from(&QApplication::staticMetaObject);
}

RubyValue engineMetaObject()
{
    return RubyValue::from(&QQmlEngine::staticMetaObject);
}

RubyValue init(RubyValue self, RubyValue argv)
{
    Q_UNUSED(self);
    Kernel::init(argv.to<QList<QByteArray>>());
    return Qnil;
}

RubyValue initialized(RubyValue self)
{
    Q_UNUSED(self);
    return RubyValue::from(Kernel::initialized());
}

}

void defineModule()
{
    RubyModule kernel(RubyModule::fromPath("QML"), "Kernel");
    kernel.defineModuleFunction("application", RUBYQML_FUNCTION_INFO(&application));
    kernel.defineModuleFunction("engine", RUBYQML_FUNCTION_INFO(&engine));
    kernel.defineModuleFunction("event_loop_hook_timer", RUBYQML_FUNCTION_INFO(&eventLoopHookTimer));
    kernel.defineModuleFunction("set_event_loop_hook_enabled_later", RUBYQML_FUNCTION_INFO(&setEventLoopHookEnabledLater));
    kernel.defineModuleFunction("application_meta_object", RUBYQML_FUNCTION_INFO(&applicationMetaObject));
    kernel.defineModuleFunction("engine_meta_object", RUBYQML_FUNCTION_INFO(&engineMetaObject));
    kernel.defineModuleFunction("init", RUBYQML_FUNCTION_INFO(&init));
    kernel.defineModuleFunction("initialized?", RUBYQML_FUNCTION_INFO(&initialized));
}

} // namespace Kernel
} // namespace RubyQml
