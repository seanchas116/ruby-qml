#pragma once

#include <stdexcept>
#include <QString>

namespace RubyQml {

class ConversionError : public std::runtime_error
{
public:
    ConversionError(const QString &str) : std::runtime_error(str.toUtf8()) {}
};

}
