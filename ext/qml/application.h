#pragma once

#include <QList>
#include <QApplication>
#include <QQmlEngine>

namespace RubyQml {
namespace Application {

QApplication *application();
QQmlEngine *engine();

void init(const QList<QByteArray> &args);

}
} // namespace RubyQml
