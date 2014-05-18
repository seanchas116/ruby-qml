#include "qmlexception.h"
#include <QString>

namespace RubyQml {

QmlException::QmlException(const QString &error) :
    std::runtime_error(error.toStdString())
{
}

} // namespace RubyQml
