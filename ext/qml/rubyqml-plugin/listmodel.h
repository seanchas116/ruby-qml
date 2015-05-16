#pragma once

#include <QAbstractListModel>
#include <QJSValue>

namespace RubyQml {

class ListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ListModel(const QJSValue &rubyModelAccess, QObject *parent = 0);

    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;

signals:

public slots:
    void beginMove(const QJSValue &first, const QJSValue &last, const QJSValue &destination);
    void endMove();
    void beginInsert(const QJSValue &first, const QJSValue &last);
    void endInsert();
    void beginRemove(const QJSValue &first, const QJSValue &last);
    void endRemove();
    void beginReset();
    void endReset();
    void update(const QJSValue &first, const QJSValue &last);

private:

    QHash<int, QByteArray> mRoleNames;
    QJSValue mRubyModelAccess;
};

} // namespace RubyQml
