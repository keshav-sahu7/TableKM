#ifndef LINEEDIT_HPP
#define LINEEDIT_HPP

#include <QLineEdit>

class LineEdit : public QLineEdit
{
    Q_OBJECT

public:
    LineEdit(const QString &text, QWidget *parent);
    LineEdit(QWidget *parent);
signals:
    void focusLost();
    void focusGained();
protected:
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;
};

inline LineEdit::LineEdit(const QString &text, QWidget *parent) : QLineEdit(text,parent)
{

}

inline LineEdit::LineEdit(QWidget *parent) : QLineEdit(parent)
{

}


#endif // LINEEDIT_HPP
