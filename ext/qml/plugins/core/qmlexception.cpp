#include "qmlexception.h"
#include <QQmlError>

namespace RubyQml {

QmlException::QmlException(const QQmlError &error) :
    std::runtime_error(error.toString().toStdString())
{
}

} // namespace RubyQml
