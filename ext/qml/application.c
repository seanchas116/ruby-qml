#include "application.h"

VALUE rbqml_cApplication = Qnil;

typedef struct {
    qmlbind_application *application;
} application_t;

static void application_free(void *p) {
    application_t *data = (application_t*)p;
    // application is never released
    // qmlbind_application_release(data->application);
    xfree(data);
}

static const rb_data_type_t data_type = {
    "QML::Application",
    { NULL, &application_free }
};

qmlbind_application *rbqml_get_application(VALUE self) {
    application_t *data;
    TypedData_Get_Struct(self, application_t, &data_type, data);
    return data->application;
}

static VALUE application_alloc(VALUE klass) {
    application_t *data = ALLOC(application_t);
    data->application = NULL;
    return TypedData_Wrap_Struct(klass, &data_type, data);
}

static VALUE application_initialize(VALUE self, VALUE args) {
    if (rb_thread_main() != rb_thread_current()) {
        rb_raise(rb_eThreadError, "Initializing QML::Application outside the main thread");
    }

    application_t *data;
    TypedData_Get_Struct(self, application_t, &data_type, data);

    if (rb_type(args) != T_ARRAY) {
        rb_raise(rb_eTypeError, "Expected Array");
    }

    args = rb_ary_concat(rb_ary_new_from_args(1, rb_argv0), args);

    int argc = RARRAY_LEN(args);
    const char **argv = malloc(argc * sizeof(char *));

    for (int i = 0; i < argc; ++i) {
        VALUE arg = RARRAY_AREF(args, i);
        argv[i] = rb_string_value_cstr(&arg);
    }

    data->application = qmlbind_application_new(argc, argv);

    return self;
}

/*
 * Starts the event loop of the application.
 * This method never returns until the application quits.
 */
static VALUE application_exec(VALUE self) {
    qmlbind_application *app = rbqml_get_application(self);
    int ret = (int)rb_thread_call_without_gvl((void *(*)(void *))&qmlbind_application_exec, app, RUBY_UBF_IO, NULL);
    return INT2NUM(ret);
}

/*
 * Processes queued events in the event loop manually.
 * This method is useful when you are combining an external event loop like EventMachine.
 */
static VALUE application_process_events(VALUE application) {
    rb_thread_call_without_gvl((void *(*)(void *))&qmlbind_process_events, NULL, RUBY_UBF_IO, NULL);
    return Qnil;
}

void rbqml_init_application(void) {
    VALUE mQML = rb_define_module("QML");
    rbqml_cApplication = rb_define_class_under(mQML, "Application", rb_cObject);
    rb_define_alloc_func(rbqml_cApplication, application_alloc);

    rb_define_private_method(rbqml_cApplication, "initialize", application_initialize, 1);
    rb_define_method(rbqml_cApplication, "exec", application_exec, 0);
    rb_define_method(rbqml_cApplication, "process_events", application_process_events, 0);
}
