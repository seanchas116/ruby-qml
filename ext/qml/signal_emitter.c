#include "signal_emitter.h"
#include "conversion.h"
#include "engine.h"

VALUE rbqml_cSignalEmitter;

typedef struct {
    qmlbind_signal_emitter emitter;
} emitter_t;

static void emitter_free(void *p) {
    emitter_t *data = (emitter_t *)p;
    qmlbind_signal_emitter_release(data->emitter);
    xfree(data);
}

static const rb_data_type_t data_type = {
    "QML::SignalEmitter",
    { NULL, &emitter_free }
};

VALUE rbqml_signal_emitter_new(qmlbind_signal_emitter emitter) {
    emitter_t *data = ALLOC(emitter_t);
    data->emitter = emitter;
    return TypedData_Wrap_Struct(rbqml_cSignalEmitter, &data_type, data);
}

static VALUE emitter_alloc(VALUE klass) {
    rb_raise(rb_eTypeError, "QML::SignalEmitter cannot be created from Ruby");
}

static VALUE emitter_emit(VALUE self, VALUE name, VALUE args) {
    emitter_t *data;
    TypedData_Get_Struct(self, emitter_t, &data_type, data);
    qmlbind_signal_emitter emitter = data->emitter;
    qmlbind_engine engine = qmlbind_signal_emitter_get_engine(emitter);

    int argc = RARRAY_LEN(args);
    qmlbind_value *qmlArgs = malloc(argc * sizeof(qmlbind_value));

    for (int i = 0; i < argc; ++i) {
        qmlbind_value value = rbqml_to_qml(RARRAY_AREF(args, i), rbqml_value_for_engine(engine));
        qmlArgs[i] = value;
    }

    qmlbind_signal_emitter_emit(data->emitter, rb_id2name(rb_sym2id(name)), argc, qmlArgs);
    return self;
}

void rbqml_init_signal_emitter(void) {
    rbqml_cSignalEmitter = rb_define_class_under(rb_path2class("QML"), "SignalEmitter", rb_cObject);
    rb_define_alloc_func(rbqml_cSignalEmitter, &emitter_alloc);
    rb_define_method(rbqml_cSignalEmitter, "emit", &emitter_emit, 2);
}
