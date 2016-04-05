#include "engine.h"
#include "conversion.h"
#include "signal_emitter.h"
#include "interface.h"

VALUE rbqml_mInterface;
static qmlbind_interface *interface;
VALUE rbqml_referenced_objects;

qmlbind_interface *rbqml_get_interface(void) {
    return interface;
}

typedef struct {
    qmlbind_backref *class_handle;
    qmlbind_signal_emitter *emitter;
} new_object_data;

static void *new_object_impl(void *p) {
    new_object_data *data = p;

    VALUE klass = (VALUE)data->class_handle;
    VALUE obj = rb_funcall(klass, rb_intern("new"), 0);
    VALUE emitterValue = rbqml_signal_emitter_new(data->emitter);
    rb_funcall(obj, rb_intern("set_signal_emitter"), 1, emitterValue);

    rb_hash_aset(rbqml_referenced_objects, obj, Qnil);
    return (void *)obj;
}

static qmlbind_backref *new_object(qmlbind_backref *class_handle, qmlbind_signal_emitter *emitter) {
    new_object_data data;
    data.class_handle = class_handle;
    data.emitter = emitter;

    return rb_thread_call_with_gvl(&new_object_impl, &data);
}

static void *delete_object_impl(void *data) {
    rb_hash_delete(rbqml_referenced_objects, (VALUE)data);
    return NULL;
}

static void delete_object(qmlbind_backref *handle) {
    rb_thread_call_with_gvl(&delete_object_impl, handle);
}

typedef struct {
    qmlbind_backref *backref;
    const char *name;
    int argc;
    const qmlbind_value *const *argv;
} call_method_data;

static void *call_method_impl(void *p) {
    call_method_data *data = p;

    VALUE obj = (VALUE)data->backref;
    VALUE method = ID2SYM(rb_intern(data->name));

    VALUE *args = alloca(data->argc * sizeof(VALUE));
    for (int i = 0; i < data->argc; ++i) {
        args[i] = rbqml_to_ruby(data->argv[i]);
    }

    VALUE result = rb_funcall(rbqml_mInterface, rb_intern("call_method"), 3,
                              obj, method, rb_ary_new_from_values(data->argc, args));

    return rbqml_to_qml(result);
}

static qmlbind_value *call_method(
    qmlbind_engine *engine,
    qmlbind_backref *object_backref, const char *name,
    int argc, const qmlbind_value *const *argv) {

    call_method_data data;
    data.backref = object_backref;
    data.name = name;
    data.argc = argc;
    data.argv = argv;

    return rb_thread_call_with_gvl(&call_method_impl, &data);
}

typedef struct {
    qmlbind_backref *backref;
    const char *name;
} get_property_data;

static void *get_property_impl(void *p) {
    get_property_data *data = p;

    VALUE obj = (VALUE)data->backref;
    VALUE method = ID2SYM(rb_intern(data->name));

    VALUE result = rb_funcall(rbqml_mInterface, rb_intern("call_method"), 3,
                              obj, method, rb_ary_new());

    return rbqml_to_qml(result);
}

static qmlbind_value *get_property(
    qmlbind_engine *engine,
    qmlbind_backref *object_backref, const char *name) {

    get_property_data data;
    data.backref = object_backref;
    data.name = name;

    return rb_thread_call_with_gvl(&get_property_impl, &data);
}

typedef struct {
    qmlbind_backref *backref;
    const char *name;
    const qmlbind_value *value;
} set_property_data;

static void *set_property_impl(void *p) {
    set_property_data* data = p;

    VALUE obj = (VALUE)data->backref;
    VALUE method = rb_str_intern(rb_sprintf("%s=", data->name));

    VALUE ruby_value = rbqml_to_ruby(data->value);

    rb_funcall(rbqml_mInterface, rb_intern("call_method"), 3,
                              obj, method, rb_ary_new_from_args(1, ruby_value));

    return NULL;
}

static void set_property(
    qmlbind_engine *engine,
    qmlbind_backref *object_backref, const char *name, const qmlbind_value *value) {

    set_property_data data;
    data.backref = object_backref;
    data.name = name;
    data.value = value;

    rb_thread_call_with_gvl(&set_property_impl, &data);
}

qmlbind_interface_handlers handlers = {
    &new_object, &delete_object, &call_method, &get_property, &set_property
};

void rbqml_init_interface(void) {
    rb_require("qml/interface");
    rbqml_mInterface = rb_path2class("QML::Interface");
    interface = qmlbind_interface_new(handlers);
    rbqml_referenced_objects = rb_hash_new();
    rb_gc_register_address(&rbqml_referenced_objects);
}
