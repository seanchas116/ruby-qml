#pragma once

#include <QObject>
#include <QQmlComponent>

class QQmlComponent;

namespace RubyQml {

class ComponentExtension : public QObject
{
    Q_OBJECT
public:
    explicit ComponentExtension(QQmlComponent *component);

public slots:

    void loadString(const QString &data, const QString &path);
    void loadFile(const QString &filePath);
    QObject *create(QQmlContext *context);

private:
    void throwIfError();

    QQmlComponent *mComponent;
};

} // namespace RubyQml
