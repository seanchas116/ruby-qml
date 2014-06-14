#pragma once

#include "extbase.h"
#include <memory>

class QPluginLoader;

namespace RubyQml {
namespace Ext {

class PluginLoader : public ExtBase<PluginLoader>
{
    friend class ExtBase<PluginLoader>;

public:
    PluginLoader();
    ~PluginLoader();

    RubyValue initialize(RubyValue path);
    RubyValue load();
    RubyValue instance();

    static void initClass();

private:
    void mark() {}
    std::unique_ptr<QPluginLoader> mPluginLoader;
};

} // namespace Ext
} // namespace RubyQml

