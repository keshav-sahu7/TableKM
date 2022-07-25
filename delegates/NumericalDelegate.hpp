#ifndef NUMERICALDELEGATE_H
#define NUMERICALDELEGATE_H

#include <QItemDelegate>

class NumericalDelegate : public QItemDelegate
{
public:
    explicit NumericalDelegate(QValidator *validator = nullptr, QObject *parent = nullptr);
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setValidator(QValidator *validator);
    void setValidator(const QString &pattern);
private:
    QValidator *m_validator;
};

inline NumericalDelegate::NumericalDelegate(QValidator *validator, QObject *parent)
    : QItemDelegate{parent},
      m_validator(validator)
{
    //left intensionally.
}

inline void NumericalDelegate::setValidator(QValidator *validator)
{
    m_validator = validator;
}




#endif // NUMERICALDELEGATE_H
