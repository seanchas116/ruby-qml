#include "engine.h"
#include "conversion.h"

static VALUE engine_class;
static qmlbind_engine global_engine;
static VALUE global_engine_value;

static void free_engine(void *engine)
{
    qmlbind_engine_release((qmlbind_engine)engine);
}

static const rb_data_type_t data_type = {
    "QML::Engine",
    { NULL, &free_engine }
};

static qmlbind_engine get_engine(VALUE self)
{
    void *engine;
    TypedData_Get_Struct(self, void, &data_type, engine);
    return (qmlbind_engine)engine;
}

static VALUE engine_evaluate(VALUE self, int argc, VALUE *argv)
{
    qmlbind_engine engine = get_engine(self);

    VALUE str, file, lineNum;

    rb_scan_args(argc, argv, "12", &str, &file, &lineNum);

    qmlbind_value value =  qmlbind_engine_eval(engine, rb_string_value_cstr(&str),
                                               RTEST(file) ? rb_string_value_cstr(&file) : "",
                                               RTEST(lineNum) ? rb_num2int(lineNum) : 1);

    return rubyqml_to_ruby(value);
}

void rubyqml_init_engine()
{
    VALUE qml_module = rb_path2class("QML");
    engine_class = rb_define_class_under(qml_module, "Engine", rb_cObject);
    rb_define_method(engine_class, "evaluate", &engine_evaluate, -1);

    global_engine = qmlbind_engine_new();
    global_engine_value = TypedData_Wrap_Struct(engine_class, &data_type, global_engine);

    rb_define_module_function(qml_module, "engine", &rubyqml_global_engine_value, 0);
}

qmlbind_engine rubyqml_global_engine(void)
{
    return global_engine;
}

VALUE rubyqml_global_engine_value(void)
{
    return global_engine_value;
}
