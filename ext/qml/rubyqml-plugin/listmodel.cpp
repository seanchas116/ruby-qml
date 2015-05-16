#include "listmodel.h"
#include <QQmlEngine>
#include <QDebug>

namespace RubyQml {

static QJSValue callMethod(QJSValue self, const QString &name, const QJSValueList &args)
{
    QJSValue func = self.property(name);
    return func.callWithInstance(self, args);
}

ListModel::ListModel(const QJSValue &rubyModelAccess, QObject *parent) : QAbstractListModel(parent),
    mRubyModelAccess(rubyModelAccess)
{
    QObject *access = rubyModelAccess.toQObject();
    Q_ASSERT(access);
    QQmlEngine::setObjectOwnership(access, QQmlEngine::CppOwnership);
    access->setParent(this);

    QVariantList columns = callMethod(rubyModelAccess, "columns", QJSValueList()).toVariant().toList();
    for (int i = 0; i < columns.size(); ++i) {
        mRoleNames[Qt::UserRole + i] = columns[i].toString().toUtf8();
    }

    connect(access, SIGNAL(begin_insert(QJSValue,QJSValue)), this, SLOT(beginInsert(QJSValue,QJSValue)));
    connect(access, SIGNAL(end_insert()), this, SLOT(endInsert()));
    connect(access, SIGNAL(begin_remove(QJSValue,QJSValue)), this, SLOT(beginRemove(QJSValue,QJSValue)));
    connect(access, SIGNAL(end_remove()), this, SLOT(endRemove()));
    connect(access, SIGNAL(begin_move(QJSValue,QJSValue,QJSValue)), this, SLOT(beginMove(QJSValue,QJSValue,QJSValue)));
    connect(access, SIGNAL(end_move()), this, SLOT(endMove()));
    connect(access, SIGNAL(update(QJSValue,QJSValue)), this, SLOT(update(QJSValue,QJSValue)));
    connect(access, SIGNAL(begin_reset()), this, SLOT(beginReset()));
    connect(access, SIGNAL(end_reset()), this, SLOT(endReset()));
}

QHash<int, QByteArray> ListModel::roleNames() const
{
    return mRoleNames;
}

QVariant ListModel::data(const QModelIndex &index, int role) const
{
    QJSValueList args;
    args << index.row();
    args << QString::fromUtf8(mRoleNames[role]);
    return QVariant::fromValue(callMethod(mRubyModelAccess, "data", args));
}

int ListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return callMethod(mRubyModelAccess, "count", QJSValueList()).toInt();
}

void ListModel::beginMove(const QJSValue &first, const QJSValue &last, const QJSValue &destination)
{
    beginMoveRows(QModelIndex(), first.toInt(), last.toInt(), QModelIndex(), destination.toInt());
}

void ListModel::endMove()
{
    endMoveRows();
}

void ListModel::beginInsert(const QJSValue &first, const QJSValue &last)
{
    beginInsertRows(QModelIndex(), first.toInt(), last.toInt());
}

void ListModel::endInsert()
{
    endInsertRows();
}

void ListModel::beginRemove(const QJSValue &first, const QJSValue &last)
{
    beginRemoveRows(QModelIndex(), first.toInt(), last.toInt());
}

void ListModel::endRemove()
{
    endRemoveRows();
}

void ListModel::beginReset()
{
    beginResetModel();
}

void ListModel::endReset()
{
    endResetModel();
}

void ListModel::update(const QJSValue &first, const QJSValue &last)
{
    emit dataChanged(index(first.toInt()), index(last.toInt()));
}

} // namespace RubyQml

