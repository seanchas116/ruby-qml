#include "js_object.h"
#include "engine.h"
#include "conversion.h"

typedef struct {
    qmlbind_value value;
} js_object_t;

VALUE rbqml_cJSObject;

void free_js_object(void *ptr)
{
    js_object_t *obj = ptr;
    qmlbind_value_release(obj->value);
    xfree(obj);
}

static const rb_data_type_t data_type = {
    "QML::JSObject",
    { NULL, &free_js_object }
};

static VALUE js_object_alloc(VALUE klass)
{
    return rbqml_js_object_new(klass, qmlbind_value_new_null());
}

VALUE rbqml_js_object_new(VALUE klass, qmlbind_value value)
{
    js_object_t *obj = ALLOC(js_object_t);
    obj->value = qmlbind_value_clone(value);
    return TypedData_Wrap_Struct(klass, &data_type, obj);
}

bool rbqml_js_object_p(VALUE value)
{
    return rb_type(value) == T_DATA && RTYPEDDATA_P(value) && RTYPEDDATA_TYPE(value) == &data_type;
}

qmlbind_value rbqml_js_object_get(VALUE jsobject)
{
    js_object_t *obj;
    TypedData_Get_Struct(jsobject, js_object_t, &data_type, obj);
    return qmlbind_value_clone(obj->value);
}

static void get_property_key(VALUE key, int *index, const char **keyStr)
{
    switch (rb_type(key)) {
    case T_FIXNUM:
        *index = NUM2INT(key);
        break;
    case T_STRING:
        *keyStr = rb_string_value_cstr(&key);
        break;
    case T_SYMBOL:
        *keyStr = rb_id2name(SYM2ID(key));
        break;
    default:
        rb_raise(rb_eTypeError, "expected Fixnum, String or Symbol for index, got %s", rb_class2name(rb_obj_class(key)));
        break;
    }
}

static VALUE js_object_aref(VALUE self, VALUE key)
{
    qmlbind_value obj = rbqml_js_object_get(self);

    int index = -1;
    const char *keyStr;
    get_property_key(key, &index, &keyStr);

    qmlbind_value value;

    if (index >= 0) {
        value = qmlbind_value_get_array_item(obj, index);
    } else {
        value = qmlbind_value_get_property(obj, keyStr);
    }

    qmlbind_value_release(obj);

    return rb_ensure(&rbqml_to_ruby, (VALUE)value, (VALUE (*)())&qmlbind_value_release, (VALUE)value);
}

static VALUE js_object_aset(VALUE self, VALUE key, VALUE value)
{
    qmlbind_value obj = rbqml_js_object_get(self);

    qmlbind_value qmlValue = rbqml_to_qml(value);

    int index = -1;
    const char *keyStr;

    get_property_key(key, &index, &keyStr);

    if (index >= 0) {
        qmlbind_value_set_array_item(obj, index, qmlValue);
    } else {
        qmlbind_value_set_property(obj, keyStr, qmlValue);
    }

    qmlbind_value_release(obj);

    return value;
}

static VALUE js_object_each_iterator(VALUE data)
{
    qmlbind_iterator it = (qmlbind_iterator)data;
    while (qmlbind_iterator_has_next(it)) {
        qmlbind_iterator_next(it);

        qmlbind_value value = qmlbind_iterator_get_value(it);
        VALUE rubyValue = rb_ensure(&rbqml_to_ruby, (VALUE)value, (VALUE (*)())&qmlbind_value_release, (VALUE)value);

        qmlbind_string str = qmlbind_iterator_get_key(it);
        VALUE rubyKey = rb_enc_str_new(qmlbind_string_get_chars(str), qmlbind_string_get_length(str), rb_utf8_encoding());
        qmlbind_string_release(str);

        VALUE pair[] = { rubyKey, rubyValue };

        rb_yield(rb_ary_new_from_values(2, pair));
    }
    return Qnil;
}

static VALUE js_object_each_pair(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, 0);

    qmlbind_value obj = rbqml_js_object_get(self);

    qmlbind_iterator it = qmlbind_iterator_new(obj);
    rb_ensure(&js_object_each_iterator, (VALUE)it, (VALUE (*)())&qmlbind_iterator_release, (VALUE)it);

    qmlbind_value_release(obj);

    return self;
}

static VALUE js_object_has_key_p(VALUE self, VALUE key)
{
    qmlbind_value obj = rbqml_js_object_get(self);

    int index = -1;
    const char *keyStr;

    get_property_key(key, &index, &keyStr);

    int ret;

    if (index >= 0) {
        ret = qmlbind_value_has_index(obj, index);
    } else {
        ret = qmlbind_value_has_property(obj, keyStr);
    }

    qmlbind_value_release(obj);

    if (ret) {
        return Qtrue;
    } else {
        return Qfalse;
    }
}

void rbqml_init_js_object(void)
{
    rbqml_cJSObject = rb_define_class_under(rb_path2class("QML"), "JSObject", rb_cObject);

    rb_define_method(rbqml_cJSObject, "[]", &js_object_aref, 1);
    rb_define_method(rbqml_cJSObject, "[]=", &js_object_aset, 2);
    rb_define_method(rbqml_cJSObject, "each_pair", &js_object_each_pair, 0);
    rb_define_alias(rbqml_cJSObject, "each", "each_pair");
    rb_define_method(rbqml_cJSObject, "has_key?", &js_object_has_key_p, 1);
}
