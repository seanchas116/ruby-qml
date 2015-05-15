#include "qml.h"
#include "conversion.h"
#include "js_object.h"
#include "js_array.h"
#include "js_function.h"
#include "js_wrapper.h"

VALUE rbqml_to_ruby(qmlbind_value value)
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
        return rb_enc_str_new(qmlbind_string_get_chars(str), qmlbind_string_get_length(str), rb_utf8_encoding());
    }

    VALUE klass;

    if (qmlbind_value_is_array(value)) {
        klass = rbqml_cJSArray;
    } else if (qmlbind_value_is_function(value)) {
        klass = rbqml_cJSFunction;
    } else if (qmlbind_value_is_wrapper(value)) {
        klass = rbqml_cJSWrapper;
    } else {
        klass = rbqml_cJSObject;
    }

    return rbqml_js_object_new(klass, value);
}

qmlbind_value rbqml_to_qml(VALUE value)
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
    case T_RATIONAL:
        return qmlbind_value_new_number(rb_float_value(rb_funcall(value, rb_intern("to_f"), 0)));
    case T_FLOAT:
        return qmlbind_value_new_number(rb_float_value(value));
    case T_STRING:
        return qmlbind_value_new_string(RSTRING_LEN(value), RSTRING_PTR(value));
    default:
        break;
    }

    return rbqml_js_object_get(value);
}
