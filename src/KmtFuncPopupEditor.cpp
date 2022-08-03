#include "KmtFuncPopupEditor.hpp"
#include "LineEdit.hpp"

#include <QApplication>
#include <QTimer>

#include "SyntaxHighlighter.hpp"
#include "misc/KDefines.h"
#include "misc/SwitchLogHandlerHelper_.hpp"


KmtFuncPopupEditor::KmtFuncPopupEditor(QWidget *parent)
    :
      QFrame(parent),
      m_table(nullptr),
      m_timer(nullptr),
      m_error(nullptr),
      m_line(8),
      m_tgt_type(km::DataType::INT32),          //ignore int32
      m_ok(false)
{
    assert(parent);

    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setLineWidth(3);
    setMidLineWidth(3);
    m_editor = new QPlainTextEdit(this);

    m_editor->setWordWrapMode(QTextOption::WordWrap);
    SyntaxHighlighter *highlighter = SyntaxHighlighter::getKMTExpressionHighlighter(this);
    highlighter->setDocument(m_editor->document());
    m_editor->setMinimumHeight(heightForLines(1));
    
    m_main_layout = new QVBoxLayout();
    m_main_layout->setContentsMargins(0,0,0,0);
    m_main_layout->setSpacing(0);
    m_main_layout->addWidget(m_editor);

    setLayout(m_main_layout);

    hide();
}

void KmtFuncPopupEditor::popup(const QPoint &pos, const QSize &size)
{
    if(isVisible())
        hidePopup();
    if(size.isValid())
        setGeometry(pos.x(),pos.y(),size.width(),size.height());
    else
        setGeometry(pos.x(),pos.y(), dynamic_cast<QWidget*>(parent())->size().width(), heightForLines(4));
    m_editor->setFocus();
    if(m_timer)
        m_timer->start();
    show();
}

void KmtFuncPopupEditor::popup(QLineEdit *line_edit, int lines)
{
    QWidget *prnt = dynamic_cast<QWidget*>(parent());
    QPoint le_point = prnt->mapFromGlobal(line_edit->mapToGlobal(QPoint(0,0)));
    QSize size = QSize(line_edit->width(), heightForLines(lines));
    QPoint point;
    if(le_point.y() + size.height() < prnt->height())
        point = le_point;//do nothing
    else if(le_point.y() + line_edit->height() - size.height() >= 0)
        point = QPoint(le_point.x(), le_point.y() + line_edit->height() - size.height());
    else
        point= QPoint(le_point.x(), 0);
    popup(point,size);
}

void KmtFuncPopupEditor::popup()
{
    QLineEdit *le = dynamic_cast<QLineEdit*>(sender());
    K_RETURN_IF_NOT(le);
    popup(le, m_line);
}

void KmtFuncPopupEditor::checkFormula()
{
    K_RETURN_IF_NOT(m_error);
    K_RETURN_IF_NOT(isModified());
    SwitchLogHandlerHelper_ _ignore_switcher([this](const std::string &str){
        auto pos = str.find(':');
        m_error->setText(str.c_str() + (pos != std::string::npos ? pos + 2 : 0));
    });
    m_ok = km::parse::getCheckedToken(getExpr().replace('\n',' ').replace('\t',' ').toStdString(), m_container, m_table, m_tgt_type);
    if(m_ok)
        m_error->clear();
    m_editor->document()->setModified(false);
}

int KmtFuncPopupEditor::heightForLines(unsigned int line) const
{
    int ht = (m_error) ? m_error->maximumHeight() : 0;
    QFontMetrics fm(font());
    return (ht + ((fm.height()+fm.lineSpacing()) * line));
}

void KmtFuncPopupEditor::hidePopup()
{
    if(!isHidden())
    {
        emit textShouldBeSaved(getExpr());
        hide();
        emit popupHidden();
        if(m_timer)
        {
            m_error->clear();
            m_timer->stop();
        }
    }
}

void KmtFuncPopupEditor::setAbsTable(const km::AbstractTable *table, km::DataType data_type)
{
    m_tgt_type = data_type;
    K_RETURN_IF(m_table == table);
    if(m_table == nullptr && table != nullptr)
    {
        m_table = table;

        m_error = new QLabel(this);
        auto font = m_error->font();
        font.setBold(true);
        m_error->setFont(font);
        m_error->setWordWrap(true);
        m_error->setStyleSheet("background-color : rgb(192,192,192);");
        auto fm = m_error->fontMetrics();
        int ht = fm.height() * 2 + fm.lineSpacing();
        m_error->setFixedHeight(ht);
        m_main_layout->addWidget(m_error);

        m_timer = new QTimer(this);
        m_timer->setInterval(1000);
        connect(m_timer, &QTimer::timeout, this, &KmtFuncPopupEditor::checkFormula);
    }
    else if(m_table != nullptr && table == nullptr)
    {
        delete m_error;
        m_error = nullptr;
        delete m_timer;
        m_timer = nullptr;
    }

    m_table = table;
}

void KmtFuncPopupEditor::setText(const QString &text)
{
    m_editor->setPlainText(text);
    m_editor->moveCursor(QTextCursor::End);
    m_editor->document()->setModified(true);
    checkFormula();
    m_editor->document()->setModified(false);
}

bool KmtFuncPopupEditor::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress && isVisible())
    {
        if(dynamic_cast<QMouseEvent*>(event)->button() == Qt::LeftButton)
            hidePopup();
    }
    return QFrame::eventFilter(watched, event);
}

void KmtFuncPopupEditor::installEventFilterOnAllChildren_v(QWidget *parent)
{
    parent->installEventFilter(this);
    for(auto child : parent->children())
    {
        if(child != this && child->isWidgetType())
        {
            installEventFilterOnAllChildren_v(dynamic_cast<QWidget*>(child));
        }
    }
}

void KmtFuncPopupEditor::installEventFilterOnAllChildren(QWidget *parent)
{
    installEventFilterOnAllChildren_v(parent);
}

void KmtFuncPopupEditor::ignoreEventsOf(const QWidgetList &list)
{
    for(auto w : list)
        w->removeEventFilter(this);
}
