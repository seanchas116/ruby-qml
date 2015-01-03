#include "conversion.h"
#include "js_object.h"
#include "js_array.h"
#include "js_function.h"

VALUE rubyqml_to_ruby(qmlbind_value value)
{
    if (qmlbind_value_is_undefined(value) || qmlbind_value_is_null(value)) {
        return Qnil;
    }
    if (qmlbind_value_is_boolean(value)) {
        return qmlbind_value_get_boolean(value) ? Qtrue : Qfalse;
    }
    if (qmlbind_value_is_number(value)) {
        double num = qmlbind_value_get_number(value);
        return rb_float_new(num);
    }
    if (qmlbind_value_is_string(value)) {
        qmlbind_string str = qmlbind_value_get_string(value);
        return rb_str_new(qmlbind_string_get_chars(str), qmlbind_string_get_length(str));
    }

    VALUE klass;

    if (qmlbind_value_is_array(value)) {
        klass = rubyqml_js_array_class();
    }
    else if (qmlbind_value_is_function(value)) {
        klass = rubyqml_js_function_class();
    }
    else {
        klass = rubyqml_js_object_class();
    }

    VALUE obj = rubyqml_js_object_alloc(klass);
    rubyqml_js_object_set(obj, value);
    return obj;
}

qmlbind_value rubyqml_to_qml(VALUE value)
{
    switch (rb_type(value)) {
    case T_NIL:
        return qmlbind_value_new_null();
    case T_TRUE:
        return qmlbind_value_new_boolean(true);
    case T_FALSE:
        return qmlbind_value_new_boolean(false);
    case T_FIXNUM:
    case T_BIGNUM:
        return qmlbind_value_new_number(NUM2LL(value));
    case T_FLOAT:
        return qmlbind_value_new_number(rb_float_value(value));
    case T_STRING:
        return qmlbind_value_new_string(RSTRING_LEN(value), RSTRING_PTR(value));
    case T_ARRAY: {
        VALUE array = rb_funcall(rubyqml_js_array_class(), rb_intern("new"), 1, value);
        return qmlbind_value_clone(rubyqml_js_object_get(array));
    }
    case T_HASH: {
        VALUE obj = rb_funcall(rubyqml_js_object_class(), rb_intern("new"), 1, value);
        return qmlbind_value_clone(rubyqml_js_object_get(obj));
    }
    default:
        if (rubyqml_is_js_object(value)) {
            return qmlbind_value_clone(rubyqml_js_object_get(value));
        }
        // TODO: support Proc
        // if (RTEST(rb_obj_is_proc(value))) {
        // }
        break;
    }

    rb_raise(rb_eTypeError, "Cannot convert %s to QML value", rb_class2name(rb_obj_class(value)));
    return qmlbind_value_new_undefined();
}
