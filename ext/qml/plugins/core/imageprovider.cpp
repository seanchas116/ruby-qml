#include "imageprovider.h"
#include "imagerequestpromise.h"
#include <QDebug>
#include <QApplication>

namespace RubyQml {

ImageProvider::ImageProvider(QObject *rubyCallback) :
    QQuickImageProvider(QQuickImageProvider::Image, QQuickImageProvider::ForceAsynchronousImageLoading),
    mRubyCallback(rubyCallback)
{
    rubyCallback->setParent(this);
}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    std::promise<QImage> promise;
    auto future = promise.get_future();
    auto promiseObj = new ImageRequestPromise(std::move(promise));
    promiseObj->moveToThread(qApp->thread());

    mRubyCallback->metaObject()->invokeMethod(
        mRubyCallback,
        "request",
        Qt::QueuedConnection,
        Q_ARG(QVariant, id),
        Q_ARG(QVariant, QVariant::fromValue(promiseObj)));

    auto image = future.get();
    if (requestedSize.isValid() && image.size() != requestedSize) {
        image = image.scaled(requestedSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    *size = image.size();
    return image;
}

} // namespace RubyQml
