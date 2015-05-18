#include "qml.h"
#include "component.h"
#include "engine.h"
#include "conversion.h"

VALUE rbqml_cComponent;

typedef struct {
    qmlbind_component component;
} component_t;

static void component_free(void *p) {
    component_t *data = (component_t *)p;
    rb_thread_call_without_gvl((void *(*)(void *))&qmlbind_component_release, data->component, RUBY_UBF_IO, NULL);
    xfree(data);
}

static const rb_data_type_t data_type = {
    "QML::Component",
    { NULL, &component_free }
};

qmlbind_component rbqml_get_component(VALUE self) {
    component_t *data;
    TypedData_Get_Struct(self, component_t, &data_type, data);
    return data->component;
}

static VALUE component_alloc(VALUE klass) {
    component_t *data = ALLOC(component_t);
    data->component = NULL;
    return TypedData_Wrap_Struct(klass, &data_type, data);
}

static VALUE component_initialize(VALUE self) {
    component_t *data;
    TypedData_Get_Struct(self, component_t, &data_type, data);
    data->component = qmlbind_component_new(rbqml_get_engine(rbqml_engine));

    return self;
}

static VALUE component_load_path(VALUE self, VALUE path) {
    qmlbind_component component = rbqml_get_component(self);
    qmlbind_component_load_path(component, rb_string_value_cstr(&path));

    return self;
}

static VALUE component_load_data(VALUE self, VALUE data, VALUE path) {
    qmlbind_component component = rbqml_get_component(self);
    qmlbind_component_set_data(component, rb_string_value_cstr(&data), rb_string_value_cstr(&path));

    return self;
}

static VALUE component_error_string(VALUE self) {
    qmlbind_component component = rbqml_get_component(self);
    qmlbind_string error = qmlbind_component_get_error_string(component);
    if (error) {
        VALUE str = rb_enc_str_new(qmlbind_string_get_chars(error), qmlbind_string_get_length(error), rb_utf8_encoding());
        qmlbind_string_release(error);
        return str;
    } else {
        return Qnil;
    }
}

static VALUE component_create(VALUE self) {
    component_t *data;
    TypedData_Get_Struct(self, component_t, &data_type, data);

    qmlbind_value obj = rb_thread_call_without_gvl((void *(*)(void *))&qmlbind_component_create, data->component, RUBY_UBF_IO, NULL);
    VALUE result = rbqml_to_ruby(obj);
    qmlbind_value_release(obj);

    return result;
}

void rbqml_init_component() {
    rbqml_cComponent = rb_define_class_under(rb_path2class("QML"), "Component", rb_cObject);
    rb_define_alloc_func(rbqml_cComponent, &component_alloc);

    rb_define_private_method(rbqml_cComponent, "initialize_impl", &component_initialize, 0);
    rb_define_private_method(rbqml_cComponent, "load_path_impl", &component_load_path, 1);
    rb_define_private_method(rbqml_cComponent, "load_data_impl", &component_load_data, 2);
    rb_define_private_method(rbqml_cComponent, "error_string", &component_error_string, 0);
    rb_define_private_method(rbqml_cComponent, "create_impl", &component_create, 0);
}
