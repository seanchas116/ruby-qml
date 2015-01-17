#include "js_function.h"
#include "conversion.h"
#include "engine.h"

VALUE rbqml_cJSFunction;

typedef struct {
    qmlbind_value *values;
    int count;
} value_list;

static void release_values(value_list *list)
{
    for (int i = 0; i < list->count; ++i) {
        qmlbind_value_release(list->values[i]);
    }
    free(list->values);
}

static VALUE js_function_call(VALUE self, int argc, VALUE *argv)
{
    qmlbind_value func = rbqml_js_object_get(self);
    VALUE engine = rbqml_js_object_get_engine(self);

    qmlbind_value *args = malloc(argc * sizeof(qmlbind_value));
    value_list list = { args, 0 };

    for (int i = 0; i < argc; ++i) {
        qmlbind_value value = (qmlbind_value)rb_ensure((VALUE(*)())&rbqml_to_qml, (VALUE)argv[i], (VALUE(*)())&release_values, (VALUE)&list);
        args[i] = value;
        list.count = i;
    }

    qmlbind_value result = qmlbind_value_call(func, argc, args);
    bool is_error = qmlbind_value_is_error(result);
    VALUE resultValue = rbqml_to_ruby(result, engine);
    qmlbind_value_release(result);

    if (is_error) {
        rb_exc_raise(rb_funcall(resultValue, rb_intern("to_error"), 0));
    }
    return resultValue;
}


void rbqml_init_js_function(void)
{
    rbqml_cJSFunction = rb_define_class_under(rb_path2class("QML"), "JSFunction", rbqml_cJSObject);

    rb_define_method(rbqml_cJSFunction, "call", &js_function_call, 0);
}
