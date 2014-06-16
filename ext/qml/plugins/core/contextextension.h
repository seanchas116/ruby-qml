#pragma once

#include <QObject>
#include <QVariant>

class QQmlContext;
class QQmlEngine;

namespace RubyQml {

class ContextExtension : public QObject
{
    Q_OBJECT
public:
    explicit ContextExtension(QQmlContext *context);

public slots:

    QQmlEngine *engine();
    QVariant evaluate(QObject *obj, const QString &str);
    void setContextProperty(const QString &key, const QVariant &value);
    QVariant contextProperty(const QString &key);

private:
    QQmlContext *mContext;
};

} // namespace RubyQml
