#pragma once

#include "markable.h"
#include "rubyvalue.h"
#include <QAbstractListModel>

namespace RubyQml {

class ListModel : public QAbstractListModel, public Markable
{
    Q_OBJECT
public:
    explicit ListModel(RubyValue rubyModel, QObject *parent = 0);
    ~ListModel();

    RubyValue rubyModel() { return mRubyModel; }

    QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;

    void gc_mark() override;

    static void defineUtilMethods();

public slots:
    void beginMove(int first, int last, int destination);
    void endMove();
    void beginInsert(int first, int last);
    void endInsert();
    void beginRemove(int first, int last);
    void endRemove();
    void beginReset();
    void endReset();
    void update(int first, int last);

private:
    RubyValue mRubyModel;
    QHash<int, QByteArray> mColumnNames;
    QHash<int, ID> mColumnIDs;
};

} // namespace RubyQml
