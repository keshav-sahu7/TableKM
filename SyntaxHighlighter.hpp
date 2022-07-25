#ifndef SYNTAXHIGHLIGHTER_HPP
#define SYNTAXHIGHLIGHTER_HPP

#include <QRegularExpression>
#include <QSyntaxHighlighter>

struct HighlightingRule{
    QTextCharFormat format;
    QRegularExpression expression;
    bool captured_group = false;
};

class SyntaxHighlighter : public QSyntaxHighlighter
{
public:
    SyntaxHighlighter(QTextDocument *document);
    SyntaxHighlighter(QObject *parent);
    void addHighlightingRule(const HighlightingRule &rule);
    ~SyntaxHighlighter();
public:
    static SyntaxHighlighter *getKMTErrorHighlighter(QObject *parent);
    static SyntaxHighlighter *getKMTExpressionHighlighter(QObject *parent);
protected:
    virtual void highlightBlock(const QString &text) override;
private:
    std::vector<HighlightingRule> m_highlighting_rules;

};

inline SyntaxHighlighter::SyntaxHighlighter(QTextDocument *document) : QSyntaxHighlighter(document)
{

}

inline SyntaxHighlighter::SyntaxHighlighter(QObject *parent) : QSyntaxHighlighter(parent)
{

}

inline void SyntaxHighlighter::addHighlightingRule(const HighlightingRule &rule)
{
    m_highlighting_rules.push_back(rule);
}




#endif // SYNTAXHIGHLIGHTER_HPP
