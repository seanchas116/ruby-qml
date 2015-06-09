#include "engine.h"
#include "conversion.h"
#include "application.h"
#include "js_object.h"
#include "js_array.h"

VALUE rbqml_cEngine;

typedef struct {
    qmlbind_engine engine;
} engine_t;

static void engine_free(void *p) {
    engine_t *data = (engine_t *)p;

    //rb_thread_call_without_gvl((void *(*)(void *))&qmlbind_engine_release, data->engine, RUBY_UBF_IO, NULL);
    xfree(data);
}

static const rb_data_type_t data_type = {
    "QML::Engine",
    { NULL, &engine_free }
};

qmlbind_engine rbqml_get_engine(VALUE self) {
    engine_t *data;
    TypedData_Get_Struct(self, engine_t, &data_type, data);
    return data->engine;
}

static VALUE engine_alloc(VALUE klass) {
    engine_t *data = ALLOC(engine_t);
    data->engine = NULL;
    return TypedData_Wrap_Struct(klass, &data_type, data);
}

static VALUE engine_initialize(VALUE self) {
    engine_t *data;
    TypedData_Get_Struct(self, engine_t, &data_type, data);
    data->engine = qmlbind_engine_new();
    return self;
}

/*
 * Adds a QML import path to the {Engine}.
 * @param path [String]
 * @see http://doc.qt.io/qt-5/qtqml-syntax-imports.html#qml-import-path
 */
static VALUE engine_add_import_path(VALUE self, VALUE path) {
    qmlbind_engine engine = rbqml_get_engine(self);
    path = rb_funcall(path, rb_intern("to_s"), 0);
    qmlbind_engine_add_import_path(engine, rb_string_value_cstr(&path));
    return self;
}

typedef struct {
    qmlbind_engine engine;
    const char *str;
    const char *file;
    int lineNum;
} evaluate_data;

static void *evaluate_impl(void *p) {
    evaluate_data *data = p;
    return qmlbind_engine_eval(data->engine, data->str, data->file, data->lineNum);
}

static VALUE engine_evaluate(VALUE self, VALUE str, VALUE file, VALUE lineNum) {
    qmlbind_engine engine = rbqml_get_engine(self);

    evaluate_data data;
    data.engine = engine;
    data.str = rb_string_value_cstr(&str);
    data.file = rb_string_value_cstr(&file);
    data.lineNum = NUM2INT(lineNum);

    qmlbind_value value = rb_thread_call_without_gvl(&evaluate_impl, &data, RUBY_UBF_IO, NULL);

    VALUE result = rbqml_to_ruby(value);
    qmlbind_value_release(value);

    return result;
}

/*
 * @paran [Integer] len
 * @return [QML::JSArray]
 */
static VALUE engine_new_array(VALUE self, VALUE len) {
    qmlbind_engine engine = rbqml_get_engine(self);

    qmlbind_value array = qmlbind_engine_new_array(engine, NUM2INT(len));
    VALUE value = rbqml_js_object_new(rbqml_cJSArray, array);
    qmlbind_value_release(array);

    return value;
}

/*
 * @return [QML::JSObject]
 */
static VALUE engine_new_object(VALUE self) {
    qmlbind_engine engine = rbqml_get_engine(self);

    qmlbind_value obj = qmlbind_engine_new_object(engine);
    VALUE value = rbqml_js_object_new(rbqml_cJSObject, obj);
    qmlbind_value_release(obj);

    return value;
}

/*
 * Starts garbage collection on the {Engine}.
 */
static VALUE engine_collect_garbage(VALUE self) {
    qmlbind_engine engine = rbqml_get_engine(self);
    qmlbind_engine_collect_garbage(engine);
    return self;
}

void rbqml_init_engine() {
    rb_require("qml/errors");

    VALUE mQML = rb_define_module("QML");

    rbqml_cEngine = rb_define_class_under(mQML, "Engine", rb_cObject);
    rb_define_alloc_func(rbqml_cEngine, &engine_alloc);

    rb_define_private_method(rbqml_cEngine, "initialize", engine_initialize, 0);
    rb_define_method(rbqml_cEngine, "add_import_path", engine_add_import_path, 1);
    rb_define_private_method(rbqml_cEngine, "evaluate_impl", engine_evaluate, 3);
    rb_define_method(rbqml_cEngine, "new_array", engine_new_array, 1);
    rb_define_method(rbqml_cEngine, "new_object", engine_new_object, 0);
    rb_define_method(rbqml_cEngine, "collect_garbage", engine_collect_garbage, 0);
}
