#ifndef DATETIMEDELEGATE_HPP
#define DATETIMEDELEGATE_HPP

#include <QItemDelegate>

class DateTimeDelegate : public QItemDelegate
{
public:
    explicit DateTimeDelegate(QObject *parent = nullptr);
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

inline DateTimeDelegate::DateTimeDelegate(QObject *parent)
    : QItemDelegate{parent}
{

}

#endif // DATETIMEDELEGATE_HPP
