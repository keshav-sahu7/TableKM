#include "HelpPage.hpp"
#include "ui_HelpPage.h"

#include <QCompleter>
#include <QFile>

#include "misc/KDefines.h"

QMap<std::string,std::string> HelpPage::map
{
    {"add", "arithmetic-function"},
    {"subtract", "arithmetic-function"},
    {"sub", "arithmetic-function"},
    {"multiply", "arithmetic-function"},
    {"mul", "arithmetic-function"},
    {"divide", "arithmetic-function"},
    {"div", "arithmetic-function"},
    {"intDiv", "arithmetic-function"},
    {"mod", "arithmetic-function"},
    {"sqrt", "arithmetic-function"},
    {"pow", "arithmetic-function"},
    {"abs", "arithmetic-function"},
    {"ceil", "arithmetic-function"},
    {"floor", "arithmetic-function"},
    {"isInRange", "arithmetic-function"},

    {"isEqual","comparison-function"},
    {"isLess","comparison-function"},
    {"isGreater","comparison-function"},
    {"isLessOrEqual","comparison-function"},
    {"isGreaterOrEqual","comparison-function"},

    {"NOT", "logical-function"},
    {"AND", "logical-function"},
    {"OR", "logical-function"},
    {"IF", "logical-function"},

    {"toInt32", "type-conversion-function"},
    {"toInt32B", "type-conversion-function"},
    {"toInt64", "type-conversion-function"},
    {"toInt64B", "type-conversion-function"},
    {"toFloat32", "type-conversion-function"},
    {"toFloat64", "type-conversion-function"},
    {"toString", "type-conversion-function"},
    {"toBoolean", "type-conversion-function"},
    {"toDate", "type-conversion-function"},
    {"toDateTime", "type-conversion-function"},

    {"length", "string-function"},
    {"concatenate", "string-function"},
    {"lowerCase", "string-function"},
    {"toLower", "string-function"},
    {"upperCase", "string-function"},
    {"toUpper", "string-function"},
    {"countChar","string-function"},
    {"contains","string-function"},
    {"containsAnyOf","string-function"},

    {"day", "date-function"},
    {"month", "date-function"},
    {"year", "date-function"},
    {"hour", "date-function"},
    {"minute", "date-function"},
    {"second", "date-function"},
    {"isLeapYear", "date-function"}
};

HelpPage::HelpPage(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::HelpPage)
{
    m_ui->setupUi(this);
    setObjectName("HelpPage");
    m_ui->browser->setSearchPaths({":/docs"});
    m_ui->browser->setSource(QUrl("index.html"));



    QStringList completion;
    completion.reserve(map.size());
    std::for_each(map.keyValueBegin(),map.keyValueEnd(), [&completion](auto pair){
        completion.append(K_SQ_STR(pair.first));
    });

    QCompleter *completer = new QCompleter(completion,this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_ui->le_search_field->setCompleter(completer);
    connect(m_ui->le_search_field, &QLineEdit::returnPressed, this, &HelpPage::searchFunction);
    connect(m_ui->btn_home, &QToolButton::clicked, this, &HelpPage::gotoHome);
    connect(m_ui->btn_search, &QToolButton::clicked, this, &HelpPage::searchFunction);

}

HelpPage::~HelpPage()
{
    delete m_ui;
}

void HelpPage::gotoHome()
{
    m_ui->browser->setSource(QUrl("index.html"));
}

void HelpPage::searchFunction()
{
    QString str = m_ui->le_search_field->text().trimmed();
    if(!str.isEmpty())
    {
        auto it = map.find(str.toStdString());
        if(it != map.end()){
            m_ui->browser->setSource(QUrl(QString("%2.html#func:%1").arg(it.key().c_str(), it.value().c_str())));
        }
    }
}

