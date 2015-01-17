#include "application.h"

VALUE rbqml_cApplication = Qnil;
VALUE rbqml_application_instance = Qnil;

typedef struct {
    qmlbind_application application;
} application_t;

static void application_free(void *p) {
    application_t *data = (application_t*)p;
    qmlbind_application_release(data->application);
    xfree(data);
}

static const rb_data_type_t data_type = {
    "QML::Application",
    { NULL, &application_free }
};

static VALUE application_init(VALUE klass, VALUE args) {
    if (rb_thread_main() != rb_thread_current()) {
        rb_raise(rb_eThreadError, "Initializing QML::Application outside the main thread");
    }

    if (rb_type(args) != T_ARRAY) {
        rb_raise(rb_eTypeError, "Expected Array");
    }

    int len = RARRAY_LEN(args);
    char **strs = malloc(len * sizeof(char *));

    for (int i = 0; i < len; ++i) {
        VALUE arg = RARRAY_AREF(args, i);
        strs[i] = rb_string_value_cstr(&arg);
    }

    application_t *data = ALLOC(application_t);
    data->application = qmlbind_application_new(len, strs);
    rbqml_application_instance = TypedData_Wrap_Struct(klass, &data_type, data);

    return rbqml_application_instance;
}

static VALUE application_instance(VALUE klass) {
    return rbqml_application_instance;
}

void rbqml_init_application(void) {
    rbqml_cApplication = rb_define_class_under(rb_path2class("QML"), "Application", rb_cObject);

    rb_define_singleton_method(rbqml_cApplication, "instance", &application_instance, 0);
    rb_define_singleton_method(rbqml_cApplication, "init", &application_init, 1);
}
