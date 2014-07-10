#include "listmodel.h"
#include "rubyclass.h"
#include <QMetaObject>
#include <QMetaMethod>

namespace RubyQml {

ListModel::ListModel(RubyValue rubyModel, QObject *parent) :
    QAbstractListModel(parent),
    mRubyModel(rubyModel)
{
    auto columns = rubyModel.send("class").send("columns").to<QList<QByteArray>>();
    for (int i = 0; i < columns.size(); ++i) {
        auto role = Qt::UserRole + i;
        mColumnNames[role] = columns[i];
        mColumnIDs[role] = rb_intern(columns[i]);
    }
    rubyModel.send("qt_models").send("push", RubyValue::fromQObject(this, false));
}

ListModel::~ListModel()
{
    mRubyModel.send("qt_models").send("delete", RubyValue::fromQObject(this, false));
}

QHash<int, QByteArray> ListModel::roleNames() const
{
    return mColumnNames;
}

QVariant ListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    return mRubyModel.send("[]", RubyValue::from(index.row())).send("[]", RubyValue::fromID(mColumnIDs[role])).to<QVariant>();
}

int ListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return mRubyModel.send("count").to<int>();
}

QModelIndex ListModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return QModelIndex();
    }
    return createIndex(row, column);
}

void ListModel::beginMove(int first, int last, int destination)
{
    beginMoveRows(QModelIndex(), first, last ,QModelIndex(), destination);
}

void ListModel::endMove()
{
    endMoveRows();
}

void ListModel::beginInsert(int first, int last)
{
    beginInsertRows(QModelIndex(), first, last);
}

void ListModel::endInsert()
{
    endInsertRows();
}

void ListModel::beginRemove(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

void ListModel::endRemove()
{
    endRemoveRows();
}

void ListModel::update(int first, int last)
{
    emit dataChanged(index(first), index(last));
}

void ListModel::gc_mark()
{
    rb_gc_mark(mRubyModel);
}

namespace {

RubyValue createWrapper(RubyValue self)
{
    return RubyValue::fromQObject(new ListModel(self), false);
}

}

void ListModel::defineUtilMethods()
{
    auto klass = RubyClass(RubyModule(RubyModule("QML"), "Data"), "ListModel");
    klass.defineMethod("create_wrapper", RUBYQML_FUNCTION_INFO(&createWrapper));
}

} // namespace RubyQml
