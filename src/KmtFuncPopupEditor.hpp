#ifndef KMTFUNCPOPUPEDITOR_HPP
#define KMTFUNCPOPUPEDITOR_HPP

#include <QFrame>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>

#include <kmt/AbstractTable.hpp>

/**
 * Shows a popup window for editing formula and provides syntax highlighting
 * and error checking system (optional).
 */
class KmtFuncPopupEditor : public QFrame
{
    Q_OBJECT
public:
    KmtFuncPopupEditor(QWidget *parent);
    int heightForLines(unsigned int line) const;
    QString getExpr() const;
    bool isFormulaVerified() const;
    void setMaxHeight(int lines);
    km::parse::TokenContainerRef getCompiledTokens();
    km::parse::ConstTokenContainerRef getCompiledTokens() const;
    void installEventFilterOnAllChildren(QWidget *parent);
    void ignoreEventsOf(const QWidgetList &list);
    QString getErrorString() const;
public slots:
    void hidePopup();
    void setAbsTable(const km::AbstractTable *table, km::DataType data_type);
    void setText(const QString &text);
    void popup(const QPoint &pos, const QSize &size = QSize());
    void popup(QLineEdit *line_edit, int lines);
    void popup();       //sender() == QLineEdit
    void checkFormula();
protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);
private:
    void installEventFilterOnAllChildren_v(QWidget *parent);
    bool isModified() const;
signals:
    void popupHidden();
    void textShouldBeSaved(const QString &text);
private:
    const km::AbstractTable *m_table;
    QTimer *m_timer;
    QPlainTextEdit *m_editor;
    QLabel *m_error;
    QVBoxLayout *m_main_layout;
    km::parse::TokenContainer m_container;
    int m_line;
    km::DataType m_tgt_type;
    bool m_ok;

};

inline QString KmtFuncPopupEditor::getExpr() const
{
    return m_editor->toPlainText();
}

inline bool KmtFuncPopupEditor::isModified() const
{
    return m_editor->document()->isModified();
}

inline bool KmtFuncPopupEditor::isFormulaVerified() const
{
    return m_ok;
}

inline void KmtFuncPopupEditor::setMaxHeight(int lines)
{
    m_line = lines;
}

inline km::parse::TokenContainerRef KmtFuncPopupEditor::getCompiledTokens()
{
    return m_container;
}

inline km::parse::ConstTokenContainerRef KmtFuncPopupEditor::getCompiledTokens() const
{
    return m_container;
}

inline QString KmtFuncPopupEditor::getErrorString() const
{
    return m_error ? m_error->text() : QString();
}

#endif // KMTFUNCPOPUPEDITOR_HPP
