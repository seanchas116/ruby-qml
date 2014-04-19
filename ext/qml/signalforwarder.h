#pragma once
#include <QtTest/QSignalSpy>
#include <ruby.h>

namespace RubyQml {

class SignalForwarder : public QSignalSpy
{
    Q_OBJECT
public:
    SignalForwarder(QObject *obj, const char *signal, VALUE proc);
    ~SignalForwarder();

private slots:
    void onSignal();

private:
    VALUE mProc;
};

} // namespace RubyQml

