#pragma once
#include <QObject>
#include <QPointer>

namespace RubyQml {

class ObjectLifeChecker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* target READ target)
public:
    explicit ObjectLifeChecker(QObject *target);
    QObject *target() { return mTarget; }
signals:

public slots:
    bool isAlive() const { return mTarget; }
    bool isOwnedByQml() const;

private:
    QPointer<QObject> mTarget;
};

} // namespace RubyQml
