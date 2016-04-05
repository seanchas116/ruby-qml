#include "exporter.h"
#include "conversion.h"
#include "application.h"
#include "interface.h"
#include "engine.h"
#include "meta_object.h"

static VALUE referenced_objects;
VALUE rbqml_cExporter;


typedef struct {
    qmlbind_exporter *exporter;
} exporter;

static void exporter_free(void *p) {
    exporter *data = (exporter *)p;

    rb_thread_call_without_gvl((void *(*)(void *))&qmlbind_exporter_release, data->exporter, RUBY_UBF_IO, NULL);
    xfree(data);
}

static const rb_data_type_t data_type = {
    "QML::Exporter",
    { NULL, &exporter_free }
};

qmlbind_exporter *rbqml_get_exporter(VALUE self) {
    exporter *data;
    TypedData_Get_Struct(self, exporter, &data_type, data);
    return data->exporter;
}

static VALUE exporter_alloc(VALUE klass) {
    exporter *data = ALLOC(exporter);
    data->exporter = NULL;
    return TypedData_Wrap_Struct(klass, &data_type, data);
}



static VALUE exporter_initialize(VALUE self, VALUE klass, VALUE name) {
    exporter *data;
    TypedData_Get_Struct(self, exporter, &data_type, data);
    data->exporter = qmlbind_exporter_new((qmlbind_backref *)klass, rb_string_value_cstr(&name), rbqml_get_interface());

    return self;
}

static VALUE exporter_add_method(VALUE self, VALUE name, VALUE arity) {
    qmlbind_exporter *exporter = rbqml_get_exporter(self);
    qmlbind_exporter_add_method(exporter, rb_id2name(SYM2ID(name)), NUM2INT(arity));
    return Qnil;
}

static VALUE exporter_add_signal(VALUE self, VALUE name, VALUE params) {
    qmlbind_exporter *exporter = rbqml_get_exporter(self);

    int arity = RARRAY_LEN(params);

    const char **paramStrs = alloca(arity * sizeof(char *));
    for (int i = 0; i < arity; ++i) {
        paramStrs[i] =  rb_id2name(SYM2ID(RARRAY_AREF(params, i)));
    }

    qmlbind_exporter_add_signal(exporter, rb_id2name(SYM2ID(name)), arity, paramStrs);
    return Qnil;
}

static VALUE exporter_add_property(VALUE self, VALUE name, VALUE notifier) {
    qmlbind_exporter *exporter = rbqml_get_exporter(self);
    qmlbind_exporter_add_property(exporter, rb_id2name(SYM2ID(name)), rb_id2name(SYM2ID(notifier)));
    return Qnil;
}

static VALUE exporter_to_metaobject(VALUE self) {
    qmlbind_exporter *exporter = rbqml_get_exporter(self);
    qmlbind_metaobject *metaobj = qmlbind_metaobject_new(exporter);
    return rbqml_metaobject_new(metaobj);
}

void rbqml_init_exporter() {
    VALUE mQML = rb_define_module("QML");
    rbqml_cExporter = rb_define_class_under(mQML, "Exporter", rb_cObject);

    rb_define_alloc_func(rbqml_cExporter, &exporter_alloc);

    rb_define_private_method(rbqml_cExporter, "initialize", exporter_initialize, 2);
    rb_define_method(rbqml_cExporter, "add_method", exporter_add_method, 2);
    rb_define_method(rbqml_cExporter, "add_signal", exporter_add_signal, 2);
    rb_define_method(rbqml_cExporter, "add_property", exporter_add_property, 2);
    rb_define_method(rbqml_cExporter, "to_meta_object", exporter_to_metaobject, 0);
}
