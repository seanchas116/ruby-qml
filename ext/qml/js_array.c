#include "js_array.h"
#include "conversion.h"
#include "engine.h"

VALUE rbqml_cJSArray;

static VALUE js_array_length(VALUE self)
{
    qmlbind_value array = rbqml_js_object_get(self);

    qmlbind_value lenValue = qmlbind_value_get_property(array, "length");
    int len = qmlbind_value_get_number(lenValue);
    qmlbind_value_release(lenValue);

    return INT2NUM(len);
}

static VALUE js_array_each(VALUE self)
{
    RETURN_SIZED_ENUMERATOR(self, 0, 0, &js_array_length);

    qmlbind_value array = rbqml_js_object_get(self);

    qmlbind_value lenValue = qmlbind_value_get_property(array, "length");
    int len = qmlbind_value_get_number(lenValue);
    qmlbind_value_release(lenValue);

    for (int i = 0; i < len; ++i) {
        qmlbind_value elem = qmlbind_value_get_array_item(array, i);
        VALUE rubyElem = rb_ensure(&rbqml_to_ruby, (VALUE)elem, (VALUE (*)())&qmlbind_value_release, (VALUE)elem);
        rb_yield(rubyElem);
    }

    return self;
}

void rbqml_init_js_array(void)
{
    rbqml_cJSArray = rb_define_class_under(rb_path2class("QML"), "JSArray", rbqml_cJSObject);

    rb_define_method(rbqml_cJSArray, "length", &js_array_length, 0);
    rb_define_method(rbqml_cJSArray, "each", &js_array_each, 0);
}
