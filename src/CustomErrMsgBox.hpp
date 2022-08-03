#ifndef CUSTOMERRMESSAGEBOX_HPP
#define CUSTOMERRMESSAGEBOX_HPP

#include <QDialog>

class QTextEdit;
class CustomErrMsgBox : public QDialog
{
    Q_OBJECT
public:
    explicit CustomErrMsgBox(const QString &title, QWidget *parent = nullptr);
    explicit CustomErrMsgBox(const QString &title, const QString &msg, QWidget *parent = nullptr);
    void setMessage(const QString &msg);
    void appendMessage(const QString &msg);
    void setHtmlMessage(const QString &msg);

    static void show(QWidget *parent,const QString &title, const QString &msg);
    static void showHtml(QWidget *parent, const QString &title, const QString &msg);
    using QDialog::show;
private:
    QTextEdit *m_msg;
};

#endif // CUSTOMERRMESSAGEBOX_HPP
