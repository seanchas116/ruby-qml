#include "ext_testutil.h"
#include "rubyclass.h"

namespace RubyQml {
namespace Ext {
namespace TestUtil {

namespace {

RubyValue echoConversion(RubyValue mod, RubyValue value)
{
    Q_UNUSED(mod);
    auto variant = RubyValue(value).to<QVariant>();
    return RubyValue::from(variant);
}

}

void defineModule()
{
    RubyModule testUtil("QML", "TestUtil");
    testUtil.toValue().defineSingletonMethod("echo_conversion", RUBYQML_FUNCTION_INFO(&echoConversion));
}

} // namespace TestUtil
} // namespace Ext
} // namespace RubyQml
