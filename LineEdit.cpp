#include "LineEdit.hpp"

#include <QFocusEvent>


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


