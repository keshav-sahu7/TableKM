#include "CustomErrMsgBox.hpp"

#include <QBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QStyle>

#include "SyntaxHighlighter.hpp"

CustomErrMsgBox::CustomErrMsgBox(const QString &title, QWidget *parent)
    : QDialog{parent}
{
    setWindowIcon(style()->standardIcon(QStyle::SP_MessageBoxCritical));
    setWindowTitle(title);
    m_msg = new QTextEdit(this);
    m_msg->setObjectName("CustomErrMsgBox_m_msg");

    // in case of light mode.
    /*
    m_msg->setFrameStyle(QFrame::NoFrame);
    QPalette palette = m_msg->palette();
    palette.setColor(QPalette::Base, this->palette().color(QPalette::Window));
    m_msg->setPalette(palette);*/

    // in case of dark mode.
    m_msg->setStyleSheet("#CustomErrMsgBox_m_msg{border:none; background-color:rgb(59,59,59);}");

    // add highlighter
    auto hltr = SyntaxHighlighter::getKMTErrorHighlighter(this);
    hltr->setDocument(m_msg->document());

    // make it read only
    m_msg->setReadOnly(true);
    m_msg->setWordWrapMode(QTextOption::WordWrap);

    // add to layout and set it to this dialog.
    QHBoxLayout *layout_btn = new QHBoxLayout();
    layout_btn->addStretch();
    QPushButton *btn_ok = new QPushButton(tr("Ok"),this);
    btn_ok->setStyleSheet("min-width: 40px;");
    connect(btn_ok, &QPushButton::clicked, this, &CustomErrMsgBox::accept);
    layout_btn->addWidget(btn_ok);

    QVBoxLayout *layout_main = new QVBoxLayout();
    layout_main->addWidget(m_msg);
    layout_main->addLayout(layout_btn);
    setLayout(layout_main);
}

CustomErrMsgBox::CustomErrMsgBox(const QString &title, const QString &msg, QWidget *parent)
    : CustomErrMsgBox(title,parent)
{
    setMessage(msg);
}

void CustomErrMsgBox::setMessage(const QString &msg)
{
    m_msg->insertPlainText(msg);
}

void CustomErrMsgBox::appendMessage(const QString &msg)
{
    m_msg->append(msg);
}

void CustomErrMsgBox::setHtmlMessage(const QString &msg)
{
    m_msg->setHtml(msg);
}

void CustomErrMsgBox::show(QWidget *parent, const QString &title, const QString &msg)
{
    CustomErrMsgBox msg_box(title,parent);
    msg_box.setMessage(msg);
    msg_box.exec();
}

void CustomErrMsgBox::showHtml(QWidget *parent, const QString &title, const QString &msg)
{
    CustomErrMsgBox msg_box(title,parent);
    msg_box.setHtmlMessage(msg);
    msg_box.exec();
}
