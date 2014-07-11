#pragma once
#include <QObject>
#include <QQuickImageProvider>

namespace RubyQml {

class ImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT
public:
    explicit ImageProvider(QObject *rubyImageProvider);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    QObject *mRubyCallback = nullptr;
};

} // namespace RubyQml
