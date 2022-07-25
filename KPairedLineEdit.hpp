#ifndef KPAIREDLINEEDIT_HPP
#define KPAIREDLINEEDIT_HPP

#include <QLineEdit>
#include <QTableWidgetItem>

class KPairedLineEdit : public QLineEdit
{
public:
    KPairedLineEdit(QLineEdit *editor, QWidget *parent = nullptr);
    KPairedLineEdit(QTableWidgetItem *item, QWidget *parent = nullptr);
    void setPair(QLineEdit *editor);
    void setPair(QTableWidgetItem *item);
    QTableWidgetItem *item() const;
    QLineEdit *editor() const;
    ~KPairedLineEdit() override = default;
private slots:
    void onTextChanged(const QString &text);
private:
    QLineEdit *m_editor;    //in case of
    QTableWidgetItem *m_item;
    QString m_current_text;
};

inline void KPairedLineEdit::setPair(QLineEdit *editor)
{
    m_editor = editor;
    m_item = nullptr;
}

inline void KPairedLineEdit::setPair(QTableWidgetItem *item)
{
    m_editor = nullptr;
    m_item = item;
}

inline QTableWidgetItem *KPairedLineEdit::item() const
{
    return m_item;
}

inline QLineEdit *KPairedLineEdit::editor() const
{
    return m_editor;
}



#endif // KPAIREDLINEEDIT_H
