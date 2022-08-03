#include "DateTimeDelegate.hpp"
#include <QDateTimeEdit>

QWidget *DateTimeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    QDateTimeEdit *date_edit = new QDateTimeEdit(parent);
    date_edit->setCalendarPopup(true);
    return date_edit;
}

void DateTimeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QDateTimeEdit *date_edit = dynamic_cast<QDateTimeEdit*>(editor);
    date_edit->setDateTime(index.model()->data(index,Qt::EditRole).toDateTime());
}

void DateTimeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDateTimeEdit *date_edit = dynamic_cast<QDateTimeEdit*>(editor);
    model->setData(index,date_edit->dateTime());
}

void DateTimeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}



