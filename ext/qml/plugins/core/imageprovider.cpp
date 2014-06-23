#include "imageprovider.h"
#include "rubycallbackloop.h"
#include <QDebug>

namespace RubyQml {

ImageProvider::ImageProvider(RubyCallbackLoop *callbackLoop, QObject *rubyCallback) :
    QQuickImageProvider(QQuickImageProvider::Image),
    mCallbackLoop(callbackLoop),
    mRubyCallback(rubyCallback)
{
    rubyCallback->setParent(this);
}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QVariant result;
    mCallbackLoop->runTask([&] {
        mRubyCallback->metaObject()->invokeMethod(
            mRubyCallback,
            "request",
            Qt::DirectConnection,
            Q_RETURN_ARG(QVariant, result),
            Q_ARG(QVariant, id));
    });

    auto data = result.toByteArray();
    auto image = QImage::fromData(data);
    if (image.isNull()) {
        qWarning() << __PRETTY_FUNCTION__ << ": invalid image returned";
    }

    if (requestedSize.isValid() && image.size() != requestedSize) {
        image = image.scaled(requestedSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    *size = image.size();
    return image;
}

} // namespace RubyQml
