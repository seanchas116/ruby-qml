#include "rubyclasses.h"
#include "rubyclass.h"

namespace RubyQml {

RubyClasses::RubyClasses()
{
    access = RubyModule("QML", "Access").toValue();
    wrapper = RubyClass("QML", "Wrapper").toValue();
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
