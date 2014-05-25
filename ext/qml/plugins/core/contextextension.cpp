#include "contextextension.h"
#include "qmlexception.h"
#include <QQmlContext>
#include <QQmlExpression>

namespace RubyQml {

ContextExtension::ContextExtension(QQmlContext *context) :
    mContext(context)
{
}

QQmlEngine *ContextExtension::engine()
{
    return mContext->engine();
}

QVariant ContextExtension::evaluate(QObject *obj, const QString &str)
{
    QQmlExpression expression(mContext, obj, str);
    auto result = expression.evaluate();
    if (expression.hasError()) {
        throw QmlException(expression.error().toString());
    }
    return result;
}

void ContextExtension::setContextProperty(const QString &key, const QVariant &value)
{
    mContext->setContextProperty(key, value);
}

QVariant ContextExtension::contextProperty(const QString &key)
{
    return mContext->contextProperty(key);
}


} // namespace RubyQml
