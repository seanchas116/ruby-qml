#include "exporter.h"
#include "conversion.h"
#include "application.h"
#include "interface.h"
#include "engine.h"

static VALUE referenced_objects;
VALUE rbqml_cExporter;


typedef struct {
    qmlbind_exporter exporter;
} exporter;

static void exporter_free(void *p) {
    exporter *data = (exporter *)p;
    qmlbind_exporter_release(data->exporter);
    xfree(data);
}

static const rb_data_type_t data_type = {
    "QML::Exporter",
    { NULL, &exporter_free }
};

qmlbind_exporter rbqml_get_exporter(VALUE self) {
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
    data->exporter = qmlbind_exporter_new((qmlbind_backref)klass, rb_string_value_cstr(&name), rbqml_get_interface());

    return self;
}

static VALUE exporter_add_method(VALUE self, VALUE name, VALUE arity) {
    qmlbind_exporter exporter = rbqml_get_exporter(self);
    qmlbind_exporter_add_method(exporter, rb_id2name(rb_sym2id(name)), NUM2INT(arity));
    return Qnil;
}

static VALUE exporter_add_signal(VALUE self, VALUE name, VALUE params) {
    qmlbind_exporter exporter = rbqml_get_exporter(self);

    int arity = RARRAY_LEN(params);

    const char **paramStrs = alloca(arity * sizeof(char *));
    for (int i = 0; i < arity; ++i) {
        paramStrs[i] =  rb_id2name(rb_sym2id(RARRAY_AREF(params, i)));
    }

    qmlbind_exporter_add_signal(exporter, rb_id2name(rb_sym2id(name)), arity, paramStrs);
    return Qnil;
}

static VALUE exporter_add_property(VALUE self, VALUE name, VALUE notifier) {
    qmlbind_exporter exporter = rbqml_get_exporter(self);
    qmlbind_exporter_add_property(exporter, rb_id2name(rb_sym2id(name)), rb_id2name(rb_sym2id(notifier)));
    return Qnil;
}

static VALUE exporter_register(VALUE self, VALUE uri, VALUE versionMajor, VALUE versionMinor, VALUE qmlName) {
    qmlbind_exporter exporter = rbqml_get_exporter(self);

    qmlbind_metaobject metaobj = qmlbind_metaobject_new(exporter);
    qmlbind_register_type(
        metaobj,
        rb_string_value_cstr(&uri),
        NUM2INT(versionMajor), NUM2INT(versionMinor),
        rb_string_value_cstr(&qmlName));

    return Qnil;
}

void rbqml_init_exporter() {
    rbqml_cExporter = rb_define_class_under(rb_path2class("QML"), "Exporter", rb_cObject);

    rb_define_alloc_func(rbqml_cExporter, &exporter_alloc);

    rb_define_private_method(rbqml_cExporter, "initialize", &exporter_initialize, 2);
    rb_define_method(rbqml_cExporter, "add_method", &exporter_add_method, 2);
    rb_define_method(rbqml_cExporter, "add_signal", &exporter_add_signal, 2);
    rb_define_method(rbqml_cExporter, "add_property", &exporter_add_property, 2);
    rb_define_method(rbqml_cExporter, "register", &exporter_register, 4);
}
