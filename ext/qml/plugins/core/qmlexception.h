#ifndef RUBYQML_QMLERROR_H
#define RUBYQML_QMLERROR_H
#include <stdexcept>
class QQmlError;

namespace RubyQml {

class QmlException : public std::runtime_error
{
public:
    QmlException(const QQmlError &error);
};

} // namespace RubyQml

#endif // RUBYQML_QMLERROR_H
