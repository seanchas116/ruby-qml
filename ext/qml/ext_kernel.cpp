#include "ext_kernel.h"
#include "application.h"
#include "rubyvalue.h"
#include "rubyclass.h"

namespace RubyQml {
namespace Ext {
namespace Kernel {

namespace {

RubyValue application()
{
    return RubyValue::from(Application::application());
}

RubyValue engine()
{
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

}

void defineModule()
{
    RubyModule kernel(RubyModule::fromPath("QML"), "Kernel");
    kernel.defineModuleFunction("application", RUBYQML_FUNCTION_INFO(&application));
    kernel.defineModuleFunction("engine", RUBYQML_FUNCTION_INFO(&engine));
    kernel.defineModuleFunction("application_meta_object", RUBYQML_FUNCTION_INFO(&applicationMetaObject));
    kernel.defineModuleFunction("engine_meta_object", RUBYQML_FUNCTION_INFO(&engineMetaObject));
}

} // namespace Kernel
} // namespace Ext
} // namespace RubyQml
