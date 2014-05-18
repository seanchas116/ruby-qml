#ifndef RUBYQML_CONTEXTWRAPPER_H
#define RUBYQML_CONTEXTWRAPPER_H

#include <QObject>
#include <QVariant>

class QQmlContext;
class QQmlEngine;

namespace RubyQml {

class ContextWrapper : public QObject
{
    Q_OBJECT
public:
    explicit ContextWrapper(QQmlContext *context);

public slots:

    QQmlContext *context() { return mContext; }
    QQmlEngine *engine();
    QVariant evaluate(QObject *obj, const QString &str);
    void setContextProperty(const QString &key, const QVariant &value);
    QVariant contextProperty(const QString &key);

private:
    QQmlContext *mContext;
};

} // namespace RubyQml

#endif // RUBYQML_CONTEXTWRAPPER_H
