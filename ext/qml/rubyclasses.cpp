#include "rubyclasses.h"
#include "rubyclass.h"

namespace RubyQml {

RubyClasses::RubyClasses()
{
    access = RubyModule(RubyModule("QML"), "Access");
    wrapper = RubyClass(RubyModule("QML"), "Wrapper");
}

static std::unique_ptr<RubyClasses> instance;

void RubyClasses::initialize()
{
    instance.reset(new RubyClasses());
}

const RubyClasses &rubyClasses()
{
    return *instance;
}

} // namespace RubyQml
