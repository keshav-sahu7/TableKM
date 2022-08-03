#include "DateDelegate.hpp"

#include <QDateEdit>



QWidget *DateDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    QDateEdit *date_edit = new QDateEdit(parent);
    date_edit->setCalendarPopup(true);
    return date_edit;
}

void DateDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QDateEdit *date_edit = dynamic_cast<QDateEdit*>(editor);
    date_edit->setDate(index.model()->data(index,Qt::EditRole).toDate());
}

void DateDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDateEdit *date_edit = dynamic_cast<QDateEdit*>(editor);
    model->setData(index,date_edit->date());
}

void DateDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}



