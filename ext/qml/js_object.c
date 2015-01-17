#include "js_object.h"
#include "engine.h"
#include "conversion.h"

typedef struct {
    qmlbind_value value;
    VALUE engine;
} js_object_t;

VALUE rbqml_cJSObject;

void mark_js_object(void *ptr)
{
    js_object_t *obj = ptr;
    rb_gc_mark(obj->engine);
}

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
    return rbqml_js_object_new(klass, qmlbind_value_new_null(), Qnil);
}

VALUE rbqml_js_object_new(VALUE klass, qmlbind_value value, VALUE engine)
{
    js_object_t *obj = ALLOC(js_object_t);
    obj->value = qmlbind_value_clone(value);
    obj->engine = engine;
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
    return obj->value;
}

VALUE rbqml_js_object_get_engine(VALUE jsobject)
{
    js_object_t *obj;
    TypedData_Get_Struct(jsobject, js_object_t, &data_type, obj);
    return obj->engine;
}

static VALUE js_object_aref(VALUE self, VALUE key)
{
    qmlbind_value obj = rbqml_js_object_get(self);
    qmlbind_value value;

    switch (rb_type(key)) {
    case T_FIXNUM:
        value = qmlbind_value_get_array_item(obj, NUM2INT(key));
        break;
    case T_STRING: {
        const char *keyStr = rb_string_value_cstr(&key);
        value = qmlbind_value_get_property(obj, keyStr);
        break;
    }
    case T_SYMBOL: {
        const char *keyStr = rb_id2name(rb_sym2id(key));
        value = qmlbind_value_get_property(obj, keyStr);
        break;
    }
    default:
        rb_raise(rb_eTypeError, "expected Fixnum, String or Symbol for index, got %s", rb_class2name(rb_obj_class(key)));
        break;
    }

    return rb_ensure(&rbqml_to_ruby, (VALUE)value, (VALUE (*)())&qmlbind_value_release, (VALUE)value);
}

static VALUE js_object_aset(VALUE self, VALUE key, VALUE value)
{
    qmlbind_value obj = rbqml_js_object_get(self);
    VALUE engine = rbqml_js_object_get_engine(self);

    qmlbind_value qmlValue = rbqml_to_qml(value, engine);

    switch (rb_type(key)) {
    case T_FIXNUM:
        qmlbind_value_set_array_item(obj, NUM2INT(key), qmlValue);
        break;
    case T_STRING: {
        const char *keyStr = rb_string_value_cstr(&key);
        qmlbind_value_set_property(obj, keyStr, qmlValue);
        break;
    }
    case T_SYMBOL: {
        const char *keyStr = rb_id2name(rb_sym2id(key));
        qmlbind_value_set_property(obj, keyStr, qmlValue);
        break;
    }
    default:
        rb_raise(rb_eTypeError, "expected Fixnum, String or Symbol for index, got %s", rb_class2name(rb_obj_class(key)));
        break;
    }

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
        VALUE rubyKey = rb_str_new(qmlbind_string_get_chars(str), qmlbind_string_get_length(str));
        qmlbind_string_release(str);

        VALUE pair[] = { rubyKey, rubyValue };

        rb_yield(rb_ary_new_from_values(2, pair));
    }
    return Qnil;
}

static VALUE js_object_each(VALUE self)
{
    RETURN_ENUMERATOR(self, 0, 0);

    qmlbind_value obj = rbqml_js_object_get(self);

    qmlbind_iterator it = qmlbind_iterator_new(obj);
    rb_ensure(&js_object_each_iterator, (VALUE)it, (VALUE (*)())&qmlbind_iterator_release, (VALUE)it);

    return self;
}

void rbqml_init_js_object(void)
{
    rbqml_cJSObject = rb_define_class_under(rb_path2class("QML"), "JSObject", rb_cObject);

    rb_define_method(rbqml_cJSObject, "[]", &js_object_aref, 1);
    rb_define_method(rbqml_cJSObject, "[]=", &js_object_aset, 2);
    rb_define_method(rbqml_cJSObject, "each", &js_object_each, 0);
}
