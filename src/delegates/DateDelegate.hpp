#ifndef DATEDELEGATE_HPP
#define DATEDELEGATE_HPP

#include <QItemDelegate>

class DateDelegate : public QItemDelegate
{
public:
    explicit DateDelegate(QObject *parent = nullptr);
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

inline DateDelegate::DateDelegate(QObject *parent)
    : QItemDelegate{parent}
{

}

#endif // DATEDELEGATE_HPP
