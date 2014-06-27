#include "ext_testutil.h"
#include "rubyclass.h"
#include "objectgc.h"

namespace RubyQml {
namespace Ext {
namespace TestUtil {

namespace {

RubyValue echoConversion(RubyValue self, RubyValue value)
{
    Q_UNUSED(self);
    auto variant = RubyValue(value).to<QVariant>();
    return RubyValue::from(variant);
}

RubyValue gcDebugMessageEnabled(RubyValue self)
{
    Q_UNUSED(self);
    return RubyValue::from(ObjectGC::instance()->debugMessageEnabled());
}

RubyValue setGCDebugMessageEnabled(RubyValue self, RubyValue enabled)
{
    ObjectGC::instance()->setDebugMessageEnabled(enabled.to<bool>());
    return gcDebugMessageEnabled(self);
}

}

void defineModule()
{
    RubyModule testUtil("QML", "TestUtil");
    testUtil.toValue().defineSingletonMethod("echo_conversion", RUBYQML_FUNCTION_INFO(&echoConversion));
    testUtil.toValue().defineSingletonMethod("gc_debug_message_enabled?", RUBYQML_FUNCTION_INFO(&gcDebugMessageEnabled));
    testUtil.toValue().defineSingletonMethod("gc_debug_message_enabled=", RUBYQML_FUNCTION_INFO(&setGCDebugMessageEnabled));
}

} // namespace TestUtil
} // namespace Ext
} // namespace RubyQml
