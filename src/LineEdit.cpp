#include "LineEdit.hpp"

#include <QFocusEvent>

LineEdit::LineEdit(const QString &text, QWidget *parent) : QLineEdit(text,parent), m_previously_empty(false)
{
    connect(this, &LineEdit::textChanged, this, &LineEdit::refreshStyleSheet);
    if(text.isEmpty())
    {
        setStyleSheet("LineEdit:!disabled { "
                      "color: rgb( 128, 128, 128);"
                      "}");
        m_previously_empty = true;
    }
}



void LineEdit::refreshStyleSheet(const QString& text)
{
    if(!text.isEmpty() && m_previously_empty)
    {
        setStyleSheet("");
    }
    else if(text.isEmpty() && !m_previously_empty)
    {
        setStyleSheet("LineEdit:!disabled { "
                      "color: rgb( 128, 128, 128);"
                      "}");
    }

    m_previously_empty = text.isEmpty();
}

void LineEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    if(event->isAccepted())
    {
        emit focusGained();
    }
}

void LineEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    if(event->isAccepted())
    {
        emit focusLost();
    }
}

