#include "dispatcher.h"

VALUE rbqml_cDispatcher;
int callback_enabled = 1;

static void tick_callback_impl() {
    VALUE dispatcher = rb_funcall(rbqml_cDispatcher, rb_intern("instance"), 0);
    rb_funcall(dispatcher, rb_intern("run_tasks"), 0);
}

static void tick_callback() {
    if (__sync_fetch_and_add(&callback_enabled, 0)) {
        rb_thread_call_with_gvl((void *(*)(void *))&tick_callback_impl, NULL);
    }
}

static VALUE dispatcher_callback_enabled_set(VALUE self, VALUE enabled) {
    if (RTEST(enabled)) {
        __sync_fetch_and_or(&callback_enabled, 1);
    } else {
        __sync_fetch_and_and(&callback_enabled, 0);
    }
    return enabled;
}

void rbqml_init_dispatcher(void) {
    rbqml_cDispatcher = rb_define_class_under(rb_path2class("QML"), "Dispatcher", rb_cObject);
    rb_define_private_method(rbqml_cDispatcher, "callback_enabled=", &dispatcher_callback_enabled_set, 1);

    qmlbind_set_tick_callback(&tick_callback);
}
