#include "listmodel.h"
#include "rubyclass.h"
#include <QMetaObject>
#include <QMetaMethod>

namespace RubyQml {

ListModel::ListModel(RubyValue rubyModel, QObject *parent) :
    QAbstractListModel(parent),
    mRubyModel(rubyModel)
{
    auto columns = rubyModel.send("columns").to<QList<QByteArray>>();
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

    QVariant result;
    withGvl([&] {
        RubyValue value;
        rescueNotify([&] {
            auto subscribe = RUBYQML_INTERN("[]");
            auto record = rb_funcall(mRubyModel, subscribe, 1, INT2NUM(index.row()));
            value = rb_funcall(record, subscribe, 1, ID2SYM(mColumnIDs[role]));
        });
        result = value.toVariant();
    });
    return result;
}

int ListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    int count;
    withGvl([&] {
        rescueNotify([&] {
            count = NUM2INT(rb_funcall(mRubyModel, RUBYQML_INTERN("count"), 0));
        });
    });
    return count;
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

void ListModel::beginReset()
{
    beginResetModel();
}

void ListModel::endReset()
{
    endResetModel();
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
