#pragma once

#include <QList>
#include <QApplication>
#include <QQmlEngine>

namespace RubyQml {
namespace Kernel {

QApplication *application();
QQmlEngine *engine();
bool initialized();

void init(const QList<QByteArray> &args);

}
} // namespace RubyQml
