#ifndef RUBYQML_QMLERROR_H
#define RUBYQML_QMLERROR_H

#include <stdexcept>
class QString;

namespace RubyQml {

class QmlException : public std::runtime_error
{
public:
    QmlException(const QString &error);
};

} // namespace RubyQml

#endif // RUBYQML_QMLERROR_H
