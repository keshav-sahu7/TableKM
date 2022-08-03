#include "KPairedLineEdit.hpp"

#include <QKeyEvent>

KPairedLineEdit::KPairedLineEdit(QLineEdit *editor, QWidget *parent) : QLineEdit(parent), m_editor(editor), m_item(nullptr), m_current_text()
{
    connect(this, &QLineEdit::textChanged, this, &KPairedLineEdit::onTextChanged);
}

KPairedLineEdit::KPairedLineEdit(QTableWidgetItem *item, QWidget *parent) : QLineEdit(parent), m_editor(nullptr), m_item(item), m_current_text()
{
    connect(this, &QLineEdit::textChanged, this, &KPairedLineEdit::onTextChanged);
}

void KPairedLineEdit::onTextChanged(const QString &text)
{
    if(m_editor && m_editor->text() == m_current_text)
        m_editor->setText(text);
    else if(m_item && m_item->text() == m_current_text)
        m_item->setText(text);
    m_current_text = text;
}







