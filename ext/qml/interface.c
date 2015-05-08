#include "engine.h"
#include "conversion.h"
#include "interface.h"

VALUE rbqml_mInterface;
static qmlbind_interface interface;
static VALUE referenced_objects;

qmlbind_interface rbqml_get_interface(void) {
    return interface;
}

static qmlbind_backref new_object(qmlbind_backref class_handle, qmlbind_signal_emitter emitter) {
    VALUE klass = (VALUE)class_handle;
    VALUE obj = rb_funcall(klass, rb_intern("new"), 0);

    rb_hash_aset(referenced_objects, obj, Qnil);
    return (qmlbind_backref)obj;
}

static void delete_object(qmlbind_backref handle) {
    VALUE obj = (VALUE)handle;
    rb_hash_delete(referenced_objects, obj);
}

static qmlbind_value call_method(
    qmlbind_engine engine,
    qmlbind_backref object_backref, const char *name,
    int argc, qmlbind_value *argv) {

    VALUE obj = (VALUE)object_backref;
    VALUE method = rb_intern(name);

    VALUE engine_value = rbqml_value_for_engine(engine);

    VALUE *args = alloca(argc * sizeof(VALUE));
    for (int i = 0; i < argc; ++i) {
        args[i] = rbqml_to_ruby(argv[i], engine_value);
    }

    VALUE result = rb_funcall(rbqml_mInterface, rb_intern("call_method"), 3,
                              obj, method,
                              rb_ary_new_from_values(argc, args));

    return rbqml_to_qml(result, engine_value);
}

static qmlbind_value get_property(
    qmlbind_engine engine,
    qmlbind_backref object_backref, const char *name) {

    VALUE obj = (VALUE)object_backref;
    VALUE method = rb_intern(name);

    VALUE engine_value = rbqml_value_for_engine(engine);

    VALUE result = rb_funcall(rbqml_mInterface, rb_intern("call_method"), 3,
                              obj, method, rb_ary_new());

    return rbqml_to_qml(result, engine_value);
}

static void set_property(
    qmlbind_engine engine,
    qmlbind_backref object_backref, const char *name, qmlbind_value value) {

    VALUE obj = (VALUE)object_backref;
    VALUE method = rb_str_intern(rb_sprintf("%s=", name));

    VALUE engine_value = rbqml_value_for_engine(engine);

    VALUE ruby_value = rbqml_to_ruby(value, engine_value);

    rb_funcall(rbqml_mInterface, rb_intern("call_method"), 3,
               obj, method, rb_ary_new_from_args(1, &ruby_value));
}

qmlbind_interface_handlers handlers = {
    &new_object, &delete_object, &call_method, &get_property, &set_property
};

void rbqml_init_interface(void) {
    rb_require("qml/interface");
    rbqml_mInterface = rb_path2class("QML::Interface");
    interface = qmlbind_interface_new(handlers);
    referenced_objects = rb_hash_new();
    rb_gc_register_address(&referenced_objects);
}
