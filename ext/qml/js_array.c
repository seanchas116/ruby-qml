#include "js_array.h"
#include "conversion.h"
#include "engine.h"

static VALUE js_array_class;

static VALUE js_array_init(VALUE self, VALUE length)
{
    int len = NUM2INT(length);
    rubyqml_js_object_set(self, qmlbind_engine_new_array(rubyqml_global_engine(), len));

    return self;
}

static VALUE js_array_length(VALUE self)
{
    qmlbind_value array = rubyqml_js_object_get(self);

    qmlbind_value lenValue = qmlbind_value_get_property(array, "length");
    int len = qmlbind_value_get_number(lenValue);
    qmlbind_value_release(lenValue);

    return INT2NUM(len);
}

static VALUE js_array_each(VALUE self)
{
    RETURN_SIZED_ENUMERATOR(self, 0, 0, &js_array_length);

    qmlbind_value array = rubyqml_js_object_get(self);

    qmlbind_value lenValue = qmlbind_value_get_property(array, "length");
    int len = qmlbind_value_get_number(lenValue);
    qmlbind_value_release(lenValue);

    for (int i = 0; i < len; ++i) {
        qmlbind_value elem = qmlbind_value_get_array_item(array, i);
        VALUE rubyElem = rb_ensure(&rubyqml_to_ruby, (VALUE)elem, (VALUE (*)())&qmlbind_value_release, (VALUE)elem);
        rb_yield(rubyElem);
    }

    return self;
}

VALUE rubyqml_js_array_class()
{
    return js_array_class;
}

void rubyqml_init_js_array(void)
{
    js_array_class = rb_define_class_under(rb_path2class("QML"), "JSArray", rubyqml_js_object_class());
    rb_define_alloc_func(js_array_class, &rubyqml_js_object_alloc);

    rb_define_method(js_array_class, "_init_array", &js_array_init, 1);
    rb_define_method(js_array_class, "each", &js_array_each, 0);
}
