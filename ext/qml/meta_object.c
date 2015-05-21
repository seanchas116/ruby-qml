#include "meta_object.h"
#include "engine.h"
#include "conversion.h"

VALUE rbqml_cMetaObject;

typedef struct {
    qmlbind_metaobject metaobject;
} metaobject;

static void metaobject_free(void *p) {
    metaobject *data = p;

    rb_thread_call_without_gvl((void *(*)(void *))&qmlbind_metaobject_release, data->metaobject, RUBY_UBF_IO, NULL);
    xfree(data);
}

static const rb_data_type_t data_type = {
    "QML::MetaObject",
    { NULL, metaobject_free }
};

qmlbind_metaobject rbqml_get_metaobject(VALUE self) {
    metaobject *data;
    TypedData_Get_Struct(self, metaobject, &data_type, data);
    return data->metaobject;
}

static VALUE metaobject_alloc(VALUE klass) {
    metaobject *data = ALLOC(metaobject);
    data->metaobject = NULL;
    return TypedData_Wrap_Struct(klass, &data_type, data);
}

VALUE rbqml_metaobject_new(qmlbind_metaobject metaobj) {
    VALUE self = metaobject_alloc(rbqml_cMetaObject);
    metaobject *data;
    TypedData_Get_Struct(self, metaobject, &data_type, data);
    data->metaobject = metaobj;
    return self;
}

typedef struct {
    qmlbind_engine engine;
    qmlbind_metaobject metaobject;
    qmlbind_backref backref;
} wrap_data;

void *wrap_impl(void *p) {
    wrap_data *data = p;
    return qmlbind_engine_new_wrapper(data->engine, data->metaobject, data->backref);
}

VALUE metaobject_wrap(VALUE self, VALUE access) {
    wrap_data data;
    data.engine = rbqml_get_engine(rbqml_engine);
    data.metaobject = rbqml_get_metaobject(self);
    data.backref = (qmlbind_backref)access;

    qmlbind_value wrapped = rb_thread_call_without_gvl(wrap_impl, &data, RUBY_UBF_IO, NULL);
    return rbqml_to_ruby(wrapped);
}

static VALUE metaobject_register(VALUE self, VALUE uri, VALUE versionMajor, VALUE versionMinor, VALUE qmlName) {
    qmlbind_metaobject metaobj = rbqml_get_metaobject(self);
    qmlbind_register_type(
        metaobj,
        rb_string_value_cstr(&uri),
        NUM2INT(versionMajor), NUM2INT(versionMinor),
        rb_string_value_cstr(&qmlName));
    return self;
}

void rbqml_init_meta_object(void) {
    VALUE mQML = rb_define_module("QML");
    rbqml_cMetaObject = rb_define_class_under(mQML, "MetaObject", rb_cObject);

    rb_define_alloc_func(rbqml_cMetaObject, metaobject_alloc);
    rb_define_method(rbqml_cMetaObject, "wrap", metaobject_wrap, 1);
    rb_define_method(rbqml_cMetaObject, "register", metaobject_register, 4);
}
