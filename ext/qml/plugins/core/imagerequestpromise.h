#pragma once
#include <QObject>
#include <QImage>
#include <future>

namespace RubyQml {

class ImageRequestPromise : public QObject
{
    Q_OBJECT
public:
    explicit ImageRequestPromise(std::promise<QImage> &&promise, QObject *parent = 0);

public slots:
    void setData(const QByteArray &data);

private:
    std::promise<QImage> mPromise;
};

} // namespace RubyQml
