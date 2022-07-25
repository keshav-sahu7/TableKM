#include "SyntaxHighlighter.hpp"

#include <QRegularExpressionMatchIterator>
#include <tuple>
#include <QDebug>

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    int n = 0;
    for(const auto &rule : m_highlighting_rules)
    {
        QRegularExpressionMatchIterator i = rule.expression.globalMatch(text);
        n = rule.captured_group ? 1 : 0;
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart(n), match.capturedLength(n), rule.format);
        }
    }
}

SyntaxHighlighter::~SyntaxHighlighter()
{
    //
}

SyntaxHighlighter * SyntaxHighlighter::getKMTErrorHighlighter(QObject *parent)
{
    SyntaxHighlighter *highlighter = new SyntaxHighlighter(parent);
    HighlightingRule rule_1;
    rule_1.expression = QRegularExpression("^.+~");
    rule_1.format.setFontWeight(QFont::Bold);
    highlighter->addHighlightingRule(rule_1);

    HighlightingRule rule_2;
    rule_2.expression = QRegularExpression("~.+Issue :");
    rule_2.format.setForeground(Qt::red);
    rule_2.format.setFontItalic(true);
    highlighter->addHighlightingRule(rule_2);

    HighlightingRule rule_3;
    rule_3.expression = QRegularExpression("`([^`\\\\]|\\\\.)*`");
    rule_3.format.setFontWeight(QFont::Bold);
    rule_3.format.setForeground(Qt::darkMagenta);
    highlighter->addHighlightingRule(rule_3);

    return highlighter;
}

SyntaxHighlighter *SyntaxHighlighter::getKMTExpressionHighlighter(QObject *parent)
{
    SyntaxHighlighter *highlighter = new SyntaxHighlighter(parent);

    HighlightingRule invalid_syntax_rule;
    invalid_syntax_rule.expression.setPattern("\\S*");
    invalid_syntax_rule.format.setUnderlineColor(Qt::red);
    invalid_syntax_rule.format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    highlighter->addHighlightingRule(invalid_syntax_rule);

    HighlightingRule par_comma_rule;
    par_comma_rule.expression.setPattern("[\\(\\),]");
    highlighter->addHighlightingRule(par_comma_rule);

    std::tuple <const char* /*Expression*/, QBrush /*Forground*/, bool /*bold*/, bool /*italic*/> highlighting_rules[] = {
        {"\\-?\\d+",Qt::darkMagenta, true, false},                      //int32
        {"\\-?\\d+(l|L)?",Qt::darkMagenta, true, false},                //int64
        {"\\-?\\d+\\.(\\d+)?f",Qt::darkMagenta, true, false},           //float32
        {"\\-?\\d+\\.(\\d+)?",Qt::darkMagenta, true, false},            //float64
        {"\"[^\"]*\"",Qt::darkGreen, false, false},                     //string
        {"(True)|(False)",Qt::darkMagenta, true, false},                //boolean
        {"\\$[[:alpha:]_]\\w*",Qt::darkBlue, true, true},         //column
    };


    for(auto &[expression, forground, bold, italic] : highlighting_rules)
    {
        HighlightingRule rule;
        rule.expression.setPattern(QString::fromLocal8Bit(expression,-1));
        rule.format.setForeground(forground);
        rule.format.setFontWeight(bold?(QFont::Bold):(QFont::Normal));
        rule.format.setFontItalic(italic);
        highlighter->addHighlightingRule(rule);
    }

    HighlightingRule function_rule;
    function_rule.expression.setPattern("([A-Za-z_][A-Za-z0-9_]*)\\s*\\(");
    function_rule.format.setFontWeight(QFont::Bold);
    function_rule.format.setFontItalic(true);
    function_rule.captured_group = true;
    highlighter->addHighlightingRule(function_rule);

    return highlighter;
}
