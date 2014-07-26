#pragma once

#include <QObject>

namespace RubyQml {

class ImageProviderTest : public QObject
{
    Q_OBJECT
public:
    explicit ImageProviderTest(QObject *parent = 0);

public slots:
    bool requestAndCompare(QObject *imageProvider, const QString &id, const QByteArray &imageData);
};

}
