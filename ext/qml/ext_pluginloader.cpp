#include "ext_pluginloader.h"
#include "ext_qtobjectpointer.h"
#include <QtCore/QPluginLoader>
#include <QtCore/QSet>

namespace RubyQml {
namespace Ext {

PluginLoader::PluginLoader() :
    mPluginLoader(new QPluginLoader())
{
}

PluginLoader::~PluginLoader()
{
}

RubyValue PluginLoader::initialize(RubyValue path)
{
    mPluginLoader->setFileName(path.to<QString>());
    return self();
}

RubyValue PluginLoader::load()
{
    auto ok = mPluginLoader->load();
    if (!ok) {
        fail("QML::PluginError", mPluginLoader->errorString());
    }
    return self();
}

RubyValue PluginLoader::instance()
{
    self().send("load");
    auto instance = mPluginLoader->instance();
    if (instance) {
        return RubyValue::from(instance);
    } else {
        return Qnil;
    }
}

void PluginLoader::initClass()
{
    ClassBuilder builder("QML", "PluginLoader");
    builder.defineMethod<METHOD_TYPE_NAME(&PluginLoader::initialize)>("initialize", MethodAccess::Private);
    builder.defineMethod<METHOD_TYPE_NAME(&PluginLoader::load)>("load");
    builder.defineMethod<METHOD_TYPE_NAME(&PluginLoader::instance)>("instance");
}

} // namespace Ext
} // namespace RubyQml
