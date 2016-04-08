#include "meta_class.h"
#include "conversion.h"
#include "application.h"
#include "engine.h"
#include "refcounter.h"
#include "signal_emitter.h"

VALUE rbqml_cMetaClass;
VALUE rbqml_mInterface;

typedef struct {
    qmlbind_client_class *class_handle;
    qmlbind_signal_emitter *emitter;
} new_object_data;

static void *new_object_impl(void *p) {
    new_object_data *data = p;

    VALUE klass = (VALUE)data->class_handle;
    VALUE obj = rb_funcall(klass, rb_intern("new"), 0);
    VALUE emitterValue = rbqml_signal_emitter_new(data->emitter);
    rb_funcall(obj, rb_intern("set_signal_emitter"), 1, emitterValue);

    rbqml_retain_value(obj);
    return (void *)obj;
}

static qmlbind_client_object *new_object(qmlbind_client_class *class_handle, qmlbind_signal_emitter *emitter) {
    new_object_data data;
    data.class_handle = class_handle;
    data.emitter = emitter;

    return rb_thread_call_with_gvl(&new_object_impl, &data);
}

static void delete_object(qmlbind_client_object *handle) {
    rb_thread_call_with_gvl((void *(*)(void *))rbqml_release_value, handle);
}

typedef struct {
    qmlbind_client_object *backref;
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
    qmlbind_client_object *handle, const char *name,
    int argc, const qmlbind_value *const *argv) {

    call_method_data data;
    data.backref = handle;
    data.name = name;
    data.argc = argc;
    data.argv = argv;

    return rb_thread_call_with_gvl(&call_method_impl, &data);
}

typedef struct {
    qmlbind_client_object *handle;
    const char *name;
} get_property_data;

static void *get_property_impl(void *p) {
    get_property_data *data = p;

    VALUE obj = (VALUE)data->handle;
    VALUE method = ID2SYM(rb_intern(data->name));

    VALUE result = rb_funcall(rbqml_mInterface, rb_intern("call_method"), 3,
                              obj, method, rb_ary_new());

    return rbqml_to_qml(result);
}

static qmlbind_value *get_property(
    qmlbind_engine *engine,
    qmlbind_client_object *handle, const char *name) {

    get_property_data data;
    data.handle = handle;
    data.name = name;

    return rb_thread_call_with_gvl(&get_property_impl, &data);
}

typedef struct {
    qmlbind_client_object *handle;
    const char *name;
    const qmlbind_value *value;
} set_property_data;

static void *set_property_impl(void *p) {
    set_property_data* data = p;

    VALUE obj = (VALUE)data->handle;
    VALUE method = rb_str_intern(rb_sprintf("%s=", data->name));

    VALUE ruby_value = rbqml_to_ruby(data->value);

    rb_funcall(rbqml_mInterface, rb_intern("call_method"), 3,
                              obj, method, rb_ary_new_from_args(1, ruby_value));

    return NULL;
}

static void set_property(
    qmlbind_engine *engine,
    qmlbind_client_object *handle, const char *name, const qmlbind_value *value) {

    set_property_data data;
    data.handle = handle;
    data.name = name;
    data.value = value;

    rb_thread_call_with_gvl(&set_property_impl, &data);
}

qmlbind_client_callbacks handlers = {
    &new_object, &delete_object, &call_method, &get_property, &set_property
};

typedef struct {
    qmlbind_metaclass *metaclass;
} meta_class;

static void meta_class_free(void *p) {
    meta_class *data = (meta_class *)p;

    rb_thread_call_without_gvl((void *(*)(void *))&qmlbind_metaclass_release, data->metaclass, RUBY_UBF_IO, NULL);
    xfree(data);
}

static const rb_data_type_t data_type = {
    "QML::MetaClass",
    { NULL, &meta_class_free }
};

qmlbind_metaclass *rbqml_get_metaclass(VALUE self) {
    meta_class *data;
    TypedData_Get_Struct(self, meta_class, &data_type, data);
    return data->metaclass;
}

static VALUE meta_class_alloc(VALUE klass) {
    meta_class *data = ALLOC(meta_class);
    data->metaclass = NULL;
    return TypedData_Wrap_Struct(klass, &data_type, data);
}

static VALUE meta_class_initialize(VALUE self, VALUE klass, VALUE name) {
    meta_class *data;
    TypedData_Get_Struct(self, meta_class, &data_type, data);
    data->metaclass = qmlbind_metaclass_new((qmlbind_client_class *)klass, rb_string_value_cstr(&name), handlers);
    rb_gc_register_mark_object(klass);

    return self;
}

static VALUE meta_class_add_method(VALUE self, VALUE name, VALUE arity) {
    qmlbind_metaclass *metaclass = rbqml_get_metaclass(self);
    qmlbind_metaclass_add_method(metaclass, rb_id2name(SYM2ID(name)), NUM2INT(arity));
    return Qnil;
}

static VALUE meta_class_add_signal(VALUE self, VALUE name, VALUE params) {
    qmlbind_metaclass *metaclass = rbqml_get_metaclass(self);

    int arity = RARRAY_LEN(params);

    const char **paramStrs = alloca(arity * sizeof(char *));
    for (int i = 0; i < arity; ++i) {
        paramStrs[i] =  rb_id2name(SYM2ID(RARRAY_AREF(params, i)));
    }

    qmlbind_metaclass_add_signal(metaclass, rb_id2name(SYM2ID(name)), arity, paramStrs);
    return Qnil;
}

static VALUE meta_class_add_property(VALUE self, VALUE name, VALUE notifier) {
    qmlbind_metaclass *metaclass = rbqml_get_metaclass(self);
    qmlbind_metaclass_add_property(metaclass, rb_id2name(SYM2ID(name)), rb_id2name(SYM2ID(notifier)));
    return Qnil;
}

typedef struct {
    qmlbind_engine *engine;
    qmlbind_metaclass *metaclass;
    qmlbind_client_object *handle;
} wrap_data;

void *wrap_impl(void *p) {
    wrap_data *data = p;
    return qmlbind_engine_new_wrapper(data->engine, data->metaclass, data->handle);
}

VALUE meta_class_wrap(VALUE self, VALUE access) {
    wrap_data data;
    data.engine = rbqml_get_engine(rbqml_engine);
    data.metaclass = rbqml_get_metaclass(self);
    data.handle = (qmlbind_client_object *)access;

    qmlbind_value *wrapped = rb_thread_call_without_gvl(wrap_impl, &data, RUBY_UBF_IO, NULL);
    VALUE ret = rbqml_to_ruby(wrapped);
    qmlbind_value_release(wrapped);
    return ret;
}

static VALUE meta_class_register(VALUE self, VALUE uri, VALUE versionMajor, VALUE versionMinor, VALUE qmlName) {
    qmlbind_metaclass *metaclass = rbqml_get_metaclass(self);
    qmlbind_metaclass_register(
        metaclass,
        rb_string_value_cstr(&uri),
        NUM2INT(versionMajor), NUM2INT(versionMinor),
        rb_string_value_cstr(&qmlName));
    return self;
}

void rbqml_init_meta_class() {
    VALUE mQML = rb_define_module("QML");
    rbqml_cMetaClass = rb_define_class_under(mQML, "MetaClass", rb_cObject);
    rb_require("qml/interface");
    rbqml_mInterface = rb_path2class("QML::Interface");

    rb_define_alloc_func(rbqml_cMetaClass, &meta_class_alloc);

    rb_define_private_method(rbqml_cMetaClass, "initialize", meta_class_initialize, 2);
    rb_define_method(rbqml_cMetaClass, "add_method", meta_class_add_method, 2);
    rb_define_method(rbqml_cMetaClass, "add_signal", meta_class_add_signal, 2);
    rb_define_method(rbqml_cMetaClass, "add_property", meta_class_add_property, 2);
    rb_define_method(rbqml_cMetaClass, "wrap", meta_class_wrap, 1);
    rb_define_method(rbqml_cMetaClass, "register", meta_class_register, 4);
}
