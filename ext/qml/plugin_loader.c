#include "plugin_loader.h"
#include "engine.h"
#include "conversion.h"

static VALUE cPluginError;
VALUE rbqml_cPluginLoader;

typedef struct {
    qmlbind_plugin plugin;
} plugin_loader_t;

static void plugin_loader_free(void *p) {
    plugin_loader_t *data = (plugin_loader_t*)p;
    qmlbind_plugin_release(data->plugin);
    xfree(data);
}

static const rb_data_type_t data_type = {
    "QML::PluginLoader",
    { NULL, &plugin_loader_free }
};

qmlbind_plugin rbqml_get_plugin(VALUE self) {
    plugin_loader_t *data;
    TypedData_Get_Struct(self, plugin_loader_t, &data_type, data);
    return data->plugin;
}

static VALUE plugin_loader_alloc(VALUE klass) {
    plugin_loader_t *data = ALLOC(plugin_loader_t);
    data->plugin = NULL;
    return TypedData_Wrap_Struct(klass, &data_type, data);
}

static VALUE plugin_loader_init(VALUE self, VALUE path) {
    plugin_loader_t *data;
    TypedData_Get_Struct(self, plugin_loader_t, &data_type, data);
    data->plugin = qmlbind_plugin_new(rb_string_value_cstr(&path));
    return self;
}

static VALUE plugin_loader_load(VALUE self) {
    qmlbind_plugin plugin = rbqml_get_plugin(self);

    qmlbind_string qmlerror = qmlbind_plugin_get_error_string(plugin);
    if (qmlerror) {
        VALUE errorStr = rb_enc_str_new(qmlbind_string_get_chars(qmlerror), qmlbind_string_get_length(qmlerror), rb_utf8_encoding());
        qmlbind_string_release(qmlerror);

        VALUE error = rb_funcall(cPluginError, rb_intern("new"), 1, errorStr);
        rb_exc_raise(error);
    }

    qmlbind_value loaded = qmlbind_plugin_get_instance(plugin, rbqml_get_engine(rbqml_engine));
    return rbqml_to_ruby(loaded);
}

void rbqml_init_plugin_loader(void) {
    cPluginError = rb_path2class("QML::PluginError");

    rbqml_cPluginLoader = rb_define_class_under(rb_path2class("QML"), "PluginLoader", rb_cObject);
    rb_define_alloc_func(rbqml_cPluginLoader, &plugin_loader_alloc);

    rb_define_private_method(rbqml_cPluginLoader, "initialize_impl", &plugin_loader_init, 1);
    rb_define_method(rbqml_cPluginLoader, "load", &plugin_loader_load, 0);
}
