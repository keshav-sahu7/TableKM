#include "NumericalDelegate.hpp"

#include <QLineEdit>



QWidget *NumericalDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    QLineEdit *line_edit = new QLineEdit(parent);
    line_edit->setValidator(m_validator);
    line_edit->setAutoFillBackground(true);
    return line_edit;
}

void NumericalDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *line_edit = dynamic_cast<QLineEdit*>(editor);
    QString data = index.model()->data(index,Qt::EditRole).toString();
    line_edit->setText(data);
}

void NumericalDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *line_edit = dynamic_cast<QLineEdit*>(editor);
    model->setData(index,line_edit->text());
}

void NumericalDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}

void NumericalDelegate::setValidator(const QString &pattern)
{
    QRegularExpression exp(pattern);
    if(!exp.isValid())
        return;
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(exp, this);
    setValidator(validator);
}


