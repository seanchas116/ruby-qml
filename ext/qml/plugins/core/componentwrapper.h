#ifndef RUBYQML_COMPONENTWRAPPER_H
#define RUBYQML_COMPONENTWRAPPER_H

#include <QObject>
#include <QQmlComponent>

class QQmlComponent;

namespace RubyQml {

class ComponentWrapper : public QObject
{
    Q_OBJECT
public:
    explicit ComponentWrapper(QQmlComponent *component);

public slots:

    void loadString(const QString &data, const QString &path);
    void loadFile(const QString &filePath);
    QObject *create();
    QQmlComponent *component() { return mComponent; }

private:
    void throwIfError();

    QQmlComponent *mComponent;
};

} // namespace RubyQml

#endif // RUBYQML_COMPONENTWRAPPER_H
