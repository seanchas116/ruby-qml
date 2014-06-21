#pragma once

#include "rubyvalue.h"
#include <memory>

class QPluginLoader;

namespace RubyQml {
namespace Ext {

class PluginLoader
{
public:
    PluginLoader(RubyValue self);
    ~PluginLoader();

    RubyValue initialize(RubyValue path);
    RubyValue load();
    RubyValue instance();

    void gc_mark() {}

    static void defineClass();

private:
    const RubyValue self;
    std::unique_ptr<QPluginLoader> mPluginLoader;
};

} // namespace Ext
} // namespace RubyQml

