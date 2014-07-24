#pragma once

#include "rubyvalue.h"
#include <memory>

class QPluginLoader;

namespace RubyQml {

class Ext_PluginLoader
{
public:
    Ext_PluginLoader(RubyValue self);
    ~Ext_PluginLoader();

    RubyValue initialize(RubyValue path);
    RubyValue load();
    RubyValue instance();

    void gc_mark() {}

    static void defineClass();

private:
    const RubyValue self;
    std::unique_ptr<QPluginLoader> mPluginLoader;
};

} // namespace RubyQml

