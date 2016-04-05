#pragma once

#include "qml.h"

extern VALUE rbqml_cSignalEmitter;

VALUE rbqml_signal_emitter_new(qmlbind_signal_emitter *emitter);

void rbqml_init_signal_emitter(void);
