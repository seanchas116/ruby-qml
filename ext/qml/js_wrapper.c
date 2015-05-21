#include "js_wrapper.h"

VALUE rbqml_cJSWrapper;

/*
 * @return [QML::Access]
 */
static VALUE js_wrapper_unwrap(VALUE self) {
    qmlbind_value wrapper = rbqml_js_object_get(self);
    VALUE unwrapped = (VALUE)qmlbind_value_get_backref(wrapper);
    qmlbind_value_release(wrapper);
    return unwrapped;
}

void rbqml_init_js_wrapper(void) {
    VALUE mQML = rb_define_module("QML");
    rbqml_cJSWrapper = rb_define_class_under(mQML, "JSWrapper", rbqml_cJSObject);
    rb_define_method(rbqml_cJSWrapper, "unwrap", js_wrapper_unwrap, 0);
}
