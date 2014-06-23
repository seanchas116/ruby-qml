#pragma once
#include <QObject>
#include <QQuickImageProvider>

namespace RubyQml {

class RubyCallbackLoop;

class ImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT
public:
    explicit ImageProvider(RubyCallbackLoop *callbackHandler, QObject *rubyImageProvider);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    RubyCallbackLoop *mCallbackLoop = nullptr;
    QObject *mRubyCallback = nullptr;
};

} // namespace RubyQml
