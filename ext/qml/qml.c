#include "application.h"
#include "component.h"
#include "engine.h"
#include "exporter.h"
#include "interface.h"
#include "js_object.h"
#include "js_array.h"
#include "js_function.h"
#include "js_wrapper.h"
#include "signal_emitter.h"

void Init_qml(void)
{
    rb_define_module("QML");

    rbqml_init_application();
    rbqml_init_engine();
    rbqml_init_component();
    rbqml_init_interface();
    rbqml_init_exporter();
    rbqml_init_js_object();
    rbqml_init_js_array();
    rbqml_init_js_function();
    rbqml_init_js_wrapper();
    rbqml_init_signal_emitter();
}
