#pragma once

#include <QList>
#include <QApplication>
#include <QQmlEngine>

namespace RubyQml {

class Kernel
{
public:
    QApplication *application() { return mApplication; }
    QQmlEngine *engine() { return mEngine; }
    QTimer *eventLoopHookTimer() { return mEventLoopHookTimer; }

    static bool initialized() { return mInstance; }
    static void init(const QList<QByteArray> &args);
    static Kernel *instance();

private:
    Kernel(const QList<QByteArray> &args);
    static Kernel *mInstance;

    QList<QByteArray> mArgData;
    int mArgc;
    char **mArgv;
    QApplication *mApplication = nullptr;
    QQmlEngine *mEngine = nullptr;
    QTimer *mEventLoopHookTimer = nullptr;
};

} // namespace RubyQml
