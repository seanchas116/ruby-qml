#pragma once

#include <QList>
#include <QApplication>
#include <QQmlEngine>

namespace RubyQml {
namespace Application {

QApplication *application();
QQmlEngine *engine();
bool initialized();

void init(const QList<QByteArray> &args);

}
} // namespace RubyQml
