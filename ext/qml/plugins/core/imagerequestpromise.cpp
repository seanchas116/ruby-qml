#include "imagerequestpromise.h"
#include <QImage>
#include <QDebug>

namespace RubyQml {

ImageRequestPromise::ImageRequestPromise(std::promise<QImage> &&promise, QObject *parent) :
    QObject(parent),
    mPromise(std::move(promise))
{
}

void ImageRequestPromise::setData(const QByteArray &data)
{
    auto image = QImage::fromData(data);
    mPromise.set_value(image);
}

} // namespace RubyQml
