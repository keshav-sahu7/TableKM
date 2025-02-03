#ifndef CUSTOMTEXTEDIT_HPP
#define CUSTOMTEXTEDIT_HPP

#include <QTextEdit>
#include <QPlainTextEdit>
#include <qcompleter.h>

class CustomTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    CustomTextEdit(QWidget *parent);
    void setCompleter(QCompleter *completer);
    QCompleter *completer() const;

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

private slots:
    void insertCompletion(const QString &completion);

private:
    QString textUnderCursor() const;

private:
    QCompleter *m_completer = nullptr;
};

#endif // CUSTOMTEXTEDIT_HPP
