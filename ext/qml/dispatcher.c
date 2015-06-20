#include "dispatcher.h"

VALUE rbqml_cDispatcher;

static void tick_callback_impl() {
    VALUE dispatcher = rb_funcall(rbqml_cDispatcher, rb_intern("instance"), 0);
    rb_funcall(dispatcher, rb_intern("run_tasks"), 0);
}

static void tick_callback() {
    rb_thread_call_with_gvl((void *(*)(void *))&tick_callback_impl, NULL);
}

static VALUE dispatcher_enable_callback(VALUE self) {
    qmlbind_set_tick_callback(&tick_callback);
    return self;
}

static VALUE dispatcher_disable_callback(VALUE self) {
    qmlbind_set_tick_callback(NULL);
    return self;
}

void rbqml_init_dispatcher(void) {
    rbqml_cDispatcher = rb_define_class_under(rb_path2class("QML"), "Dispatcher", rb_cObject);
    rb_define_private_method(rbqml_cDispatcher, "enable_callback", dispatcher_enable_callback, 0);
    rb_define_private_method(rbqml_cDispatcher, "disable_callback", dispatcher_disable_callback, 0);
}
