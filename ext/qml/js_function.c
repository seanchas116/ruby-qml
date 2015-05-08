#include "js_function.h"
#include "conversion.h"
#include "engine.h"

VALUE rbqml_cJSFunction;

static VALUE js_function_call(int argc, VALUE *argv, VALUE self) {
    qmlbind_value func = rbqml_js_object_get(self);
    VALUE engine = rbqml_js_object_get_engine(self);

    qmlbind_value *args = malloc(argc * sizeof(qmlbind_value));

    for (int i = 0; i < argc; ++i) {
        qmlbind_value value = rbqml_to_qml(argv[i], engine);
        args[i] = value;
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

void rbqml_init_js_function(void) {
    rbqml_cJSFunction = rb_define_class_under(rb_path2class("QML"), "JSFunction", rbqml_cJSObject);

    rb_define_method(rbqml_cJSFunction, "call", &js_function_call, -1);
}
