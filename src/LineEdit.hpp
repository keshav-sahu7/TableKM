#ifndef LINEEDIT_HPP
#define LINEEDIT_HPP

#include <QLineEdit>

/**
 * This class provides a signal "focusGained()" that we can use with
 * formula popup editor, it also provides a way to set stylesheet for
 * place holder text which is not available in the theme.
 */
class LineEdit : public QLineEdit
{
    Q_OBJECT

public:
    LineEdit(const QString &text, QWidget *parent);
    LineEdit(QWidget *parent);
private slots:
    void refreshStyleSheet(const QString &text);
signals:
    void focusLost();
    void focusGained();
protected:
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;
private:
    bool m_previously_empty;
};

inline LineEdit::LineEdit(QWidget *parent) : LineEdit(QString(), parent)
{

}






#endif // LINEEDIT_HPP
