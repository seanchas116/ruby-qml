#include "js_object.h"
#include "engine.h"
#include "conversion.h"

static VALUE js_object_class;

void rubyqml_free_js_object(void *ptr)
{
    rubyqml_js_object_t *obj = ptr;
    qmlbind_value_release(obj->value);
    xfree(obj);
}

static const rb_data_type_t data_type = {
    "QML::JSObject",
    { NULL, &rubyqml_free_js_object }
};

VALUE rubyqml_js_object_alloc(VALUE klass)
{
    rubyqml_js_object_t *obj = ALLOC(rubyqml_js_object_t);
    obj->value = qmlbind_value_new_null();
    return TypedData_Wrap_Struct(klass, &data_type, obj);
}

bool rubyqml_is_js_object(VALUE value)
{
    return rb_type(value) == T_DATA && RTYPEDDATA_P(value) && RTYPEDDATA_TYPE(value) == &data_type;
}

qmlbind_value rubyqml_js_object_get(VALUE jsobject)
{
    rubyqml_js_object_t *obj;
    TypedData_Get_Struct(jsobject, rubyqml_js_object_t, &data_type, obj);
    return obj->value;
}

void rubyqml_js_object_set(VALUE jsobject, qmlbind_value value)
{
    rubyqml_js_object_t *obj;
    TypedData_Get_Struct(jsobject, rubyqml_js_object_t, &data_type, obj);
    qmlbind_value_release(obj->value);
    obj->value = value;
}


static VALUE js_object_init(VALUE self)
{
    qmlbind_value obj = qmlbind_engine_new_object(rubyqml_global_engine());
    rubyqml_js_object_set(self, obj);

    return self;
}



static VALUE js_object_aref(VALUE self, VALUE key)
{
    qmlbind_value obj = rubyqml_js_object_get(self);
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

    return rb_ensure(&rubyqml_to_ruby, (VALUE)value, (VALUE (*)())&qmlbind_value_release, (VALUE)value);
}

static VALUE js_object_aset(VALUE self, VALUE key, VALUE value)
{
    qmlbind_value obj = rubyqml_js_object_get(self);
    qmlbind_value qmlValue = rubyqml_to_qml(value);

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
        VALUE rubyValue = rb_ensure(&rubyqml_to_ruby, (VALUE)value, (VALUE (*)())&qmlbind_value_release, (VALUE)value);

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

    qmlbind_value obj = rubyqml_js_object_get(self);

    qmlbind_iterator it = qmlbind_iterator_new(obj);
    rb_ensure(&js_object_each_iterator, (VALUE)it, (VALUE (*)())&qmlbind_iterator_release, (VALUE)it);

    return self;
}

void rubyqml_init_js_object(void)
{
    js_object_class = rb_define_class_under(rb_path2class("QML"), "JSObject", rb_cObject);
    rb_define_alloc_func(js_object_class, &rubyqml_js_object_alloc);

    rb_define_method(js_object_class, "_init_object", &js_object_init, 0);
    rb_define_method(js_object_class, "[]", &js_object_aref, 1);
    rb_define_method(js_object_class, "[]=", &js_object_aset, 2);
    rb_define_method(js_object_class, "each", &js_object_each, 0);
}
