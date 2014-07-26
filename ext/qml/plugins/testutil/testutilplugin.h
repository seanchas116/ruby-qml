#pragma once
#include <QObject>
#include <QVariant>

class QQuickWindow;

namespace RubyQml {

class TestObject;
class TestObjectSubclass;
class OwnershipTest;
class ObjectLifeChecker;
class ImageProviderTest;

class TestUtilPlugin : public QObject
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.ruby-qml.RubyQml.TestObject")
public:
    explicit TestUtilPlugin(QObject *parent = 0);
signals:

public slots:
    QVariantHash metaObjects() { return mMetaObjects; }

    RubyQml::TestObject *createTestObject();
    RubyQml::TestObjectSubclass *createTestObjectSubclass();
    RubyQml::OwnershipTest *createOwnershipTest();
    RubyQml::ObjectLifeChecker *createObjectLifeChecker(QObject *target);
    RubyQml::ImageProviderTest *createImageProviderTest();

private:
    QVariantHash mMetaObjects;
};

} // namespace RubyQml
