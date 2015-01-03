#include "js_function.h"
#include "conversion.h"

static VALUE js_function_class;

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
    qmlbind_value func = rubyqml_js_object_get(self);
    qmlbind_value *args = malloc(argc * sizeof(qmlbind_value));
    value_list list = { args, 0 };

    for (int i = 0; i < argc; ++i) {
        qmlbind_value value = (qmlbind_value)rb_ensure((VALUE(*)())&rubyqml_to_qml, (VALUE)argv[i], (VALUE(*)())&release_values, (VALUE)&list);
        args[i] = value;
        list.count = i;
    }

    qmlbind_value result = qmlbind_value_call(func, argc, args);
    bool is_error = qmlbind_value_is_error(result);
    VALUE resultValue = rubyqml_to_ruby(result);
    qmlbind_value_release(result);

    if (is_error) {
        rb_exc_raise(rb_funcall(resultValue, rb_intern("to_error"), 0));
    }
    return resultValue;
}

VALUE rubyqml_js_function_class(void)
{
    return js_function_class;
}

void rubyqml_init_js_function(void)
{
    js_function_class = rb_define_class_under(rb_path2class("QML"), "JSFunction", rubyqml_js_object_class());
    rb_define_alloc_func(js_function_class, &rubyqml_js_object_alloc);

    rb_define_method(js_function_class, "call", &js_function_call, 0);
}
