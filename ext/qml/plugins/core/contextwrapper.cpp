#include "contextwrapper.h"
#include "qmlexception.h"
#include <QQmlContext>
#include <QQmlExpression>

namespace RubyQml {

ContextWrapper::ContextWrapper(QQmlContext *context) :
    mContext(context)
{
}

QQmlEngine *ContextWrapper::engine()
{
    return mContext->engine();
}

QVariant ContextWrapper::evaluate(QObject *obj, const QString &str)
{
    QQmlExpression expression(mContext, obj, str);
    auto result = expression.evaluate();
    if (expression.hasError()) {
        throw QmlException(expression.error().toString());
    }
    return result;
}

void ContextWrapper::setContextProperty(const QString &key, const QVariant &value)
{
    mContext->setContextProperty(key, value);
}

QVariant ContextWrapper::contextProperty(const QString &key)
{
    return mContext->contextProperty(key);
}


} // namespace RubyQml
