#include "rubyclasses.h"
#include "rubyclass.h"

namespace RubyQml {

RubyClasses::RubyClasses()
{
    access = RubyModule("QML", "Access").toValue();
    qtObjectBase = RubyClass("QML", "QtObjectBase").toValue();
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
