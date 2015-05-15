#include "js_function.h"
#include "conversion.h"
#include "engine.h"

VALUE rbqml_cJSFunction;

typedef enum {
    CallFunction, CallMethod, CallConstructor
} CallType;

static VALUE function_call(VALUE self, VALUE thisValue, VALUE args, CallType callType) {
    qmlbind_value func = rbqml_js_object_get(self);

    int argc = RARRAY_LEN(args);
    qmlbind_value *qmlArgs = malloc(argc * sizeof(qmlbind_value));

    for (int i = 0; i < argc; ++i) {
        qmlbind_value value = rbqml_to_qml(RARRAY_AREF(args, i));
        qmlArgs[i] = value;
    }

    qmlbind_value result;

    switch (callType) {
    case CallFunction:
        result = qmlbind_value_call(func, argc, qmlArgs);
        break;
    case CallMethod: {
        qmlbind_value qmlThis = rbqml_js_object_get(thisValue);
        result = qmlbind_value_call_with_instance(func, qmlThis, argc, qmlArgs);
        qmlbind_value_release(qmlThis);
        break;
    }
    case CallConstructor: {
        result = qmlbind_value_call_constructor(func, argc, qmlArgs);
        break;
    }
    }

    bool is_error = qmlbind_value_is_error(result);
    VALUE resultValue = rbqml_to_ruby(result);
    qmlbind_value_release(result);
    qmlbind_value_release(func);

    if (is_error) {
        rb_exc_raise(rb_funcall(resultValue, rb_intern("to_error"), 0));
    }
    return resultValue;
}

static VALUE js_function_call(VALUE self, VALUE args) {
    return function_call(self, Qnil, args, CallFunction);
}

static VALUE js_function_new(VALUE self, VALUE args) {
    return function_call(self, Qnil, args, CallConstructor);
}

static VALUE js_function_call_with_instance(int argc, VALUE *argv, VALUE self) {
    VALUE thisValue;
    VALUE args;

    rb_scan_args(argc, argv, "1*", &thisValue, &args);
    return function_call(self, thisValue, args, CallMethod);
}

void rbqml_init_js_function(void) {
    rbqml_cJSFunction = rb_define_class_under(rb_path2class("QML"), "JSFunction", rbqml_cJSObject);

    rb_define_method(rbqml_cJSFunction, "call", &js_function_call, -2);
    rb_define_method(rbqml_cJSFunction, "new", &js_function_new, -2);
    rb_define_method(rbqml_cJSFunction, "call_with_instance", &js_function_call_with_instance, -1);
}
