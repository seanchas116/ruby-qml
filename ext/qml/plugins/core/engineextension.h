#pragma once

#include <QObject>
#include <QVariantList>

class QQmlEngine;
class QQmlContext;

namespace RubyQml {

class ImageProvider;

class EngineExtension : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlContext* rootContext READ rootContext)
public:
    explicit EngineExtension(QQmlEngine *engine);
    QQmlContext *rootContext();

public slots:
    void addImageProvider(const QString &id, RubyQml::ImageProvider *provider);
    void addImportPath(const QString &path);
    QVariantList importPaths() const;
    void collectGarbage();

private:
    QQmlEngine *mEngine;
};

} // namespace RubyQml
