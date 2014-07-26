#include "imageprovidertest.h"
#include <QQuickImageProvider>
#include <QCoreApplication>
#include <future>
#include <atomic>

namespace RubyQml {

ImageProviderTest::ImageProviderTest(QObject *parent) :
    QObject(parent)
{
}

bool ImageProviderTest::requestAndCompare(QObject *imageProvider, const QString &id, const QByteArray &imageData)
{
    auto provider = dynamic_cast<QQuickImageProvider *>(imageProvider);
    if (!provider) {
        throw std::runtime_error("invalid iamge provider");
    }
    std::atomic_bool finished(false);
    auto result = std::async(std::launch::async, [=, &finished] {
        QSize size;
        auto result = provider->requestImage(id, &size, QSize());
        finished = true;
        return result;
    });
    auto start = std::chrono::steady_clock::now();
    while (!finished) {
        QCoreApplication::processEvents();
        auto current = std::chrono::steady_clock::now();
        if (current - start > std::chrono::seconds(1)) {
            throw std::runtime_error("image request timeout");
        }
    }
    auto actual = result.get();
    auto expected = QImage::fromData(imageData);
    return actual == expected;
}

}
