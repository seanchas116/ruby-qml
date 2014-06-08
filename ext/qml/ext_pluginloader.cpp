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

VALUE PluginLoader::initialize(VALUE path)
{
    mPluginLoader->setFileName(fromRuby<QString>(path));
    return self();
}

VALUE PluginLoader::load()
{
    auto ok = mPluginLoader->load();
    if (!ok) {
        fail("QML::PluginError", mPluginLoader->errorString());
    }
    return self();
}

VALUE PluginLoader::instance()
{
    send(self(), "load");
    auto instance = mPluginLoader->instance();
    if (instance) {
        return toRuby<QObject *>(instance);
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
