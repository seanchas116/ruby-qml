#include "ext_kernel.h"
#include "application.h"
#include "rubyvalue.h"
#include "rubyclass.h"

namespace RubyQml {
namespace Ext_Kernel {

namespace {

RubyValue application(RubyValue self)
{
    Q_UNUSED(self);
    return RubyValue::from(Application::application());
}

RubyValue engine(RubyValue self)
{
    Q_UNUSED(self);
    return RubyValue::from(Application::engine());
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
    Application::init(argv.to<QList<QByteArray>>());
    return Qnil;
}

RubyValue initialized(RubyValue self)
{
    Q_UNUSED(self);
    return RubyValue::from(Application::initialized());
}

}

void defineModule()
{
    RubyModule kernel(RubyModule::fromPath("QML"), "Kernel");
    kernel.defineModuleFunction("application", RUBYQML_FUNCTION_INFO(&application));
    kernel.defineModuleFunction("engine", RUBYQML_FUNCTION_INFO(&engine));
    kernel.defineModuleFunction("application_meta_object", RUBYQML_FUNCTION_INFO(&applicationMetaObject));
    kernel.defineModuleFunction("engine_meta_object", RUBYQML_FUNCTION_INFO(&engineMetaObject));
    kernel.defineModuleFunction("init", RUBYQML_FUNCTION_INFO(&init));
    kernel.defineModuleFunction("initialized?", RUBYQML_FUNCTION_INFO(&initialized));
}

} // namespace Kernel
} // namespace RubyQml
