#include "HelpPage.hpp"
#include "ui_HelpPage.h"

#include <QCompleter>
#include <QFile>

#include "ThemeHandler.hpp"
#include "misc/KDefines.h"

QMap<std::string,std::string> HelpPage::map
{
    {"add", "arithmetic-function.html#func:"},
    {"subtract", "arithmetic-function.html#func:"},
    {"sub", "arithmetic-function.html#func:"},
    {"multiply", "arithmetic-function.html#func:"},
    {"mul", "arithmetic-function.html#func:"},
    {"divide", "arithmetic-function.html#func:"},
    {"div", "arithmetic-function.html#func:"},
    {"intDiv", "arithmetic-function.html#func:"},
    {"mod", "arithmetic-function.html#func:"},
    {"sqrt", "arithmetic-function.html#func:"},
    {"pow", "arithmetic-function.html#func:"},
    {"abs", "arithmetic-function.html#func:"},
    {"ceil", "arithmetic-function.html#func:"},
    {"floor", "arithmetic-function.html#func:"},
    {"isInRange", "arithmetic-function.html#func:"},

    {"isEqual","comparison-function.html#func:"},
    {"isLess","comparison-function.html#func:"},
    {"isGreater","comparison-function.html#func:"},
    {"isLessOrEqual","comparison-function.html#func:"},
    {"isGreaterOrEqual","comparison-function.html#func:"},

    {"NOT", "logical-function.html#func:"},
    {"AND", "logical-function.html#func:"},
    {"OR", "logical-function.html#func:"},
    {"IF", "logical-function.html#func:"},

    {"toInt32", "type-conversion-function.html#func:"},
    {"toInt32B", "type-conversion-function.html#func:"},
    {"toInt64", "type-conversion-function.html#func:"},
    {"toInt64B", "type-conversion-function.html#func:"},
    {"toFloat32", "type-conversion-function.html#func:"},
    {"toFloat64", "type-conversion-function.html#func:"},
    {"toString", "type-conversion-function.html#func:"},
    {"toBoolean", "type-conversion-function.html#func:"},
    {"toDate", "type-conversion-function.html#func:"},
    {"toDateTime", "type-conversion-function.html#func:"},

    {"length", "string-function.html#func:"},
    {"concatenate", "string-function.html#func:"},
    {"lowerCase", "string-function.html#func:"},
    {"toLower", "string-function.html#func:"},
    {"upperCase", "string-function.html#func:"},
    {"toUpper", "string-function.html#func:"},
    {"countChar","string-function.html#func:"},
    {"contains","string-function.html#func:"},
    {"containsAnyOf","string-function.html#func:"},

    {"day", "date-function.html#func:"},
    {"month", "date-function.html#func:"},
    {"year", "date-function.html#func:"},
    {"hour", "date-function.html#func:"},
    {"minute", "date-function.html#func:"},
    {"second", "date-function.html#func:"},
    {"isLeapYear", "date-function.html#func:"},

    {"int32", "types.html#type:"},
    {"int64", "types.html#type:"},
    {"float32", "types.html#type:"},
    {"float64", "types.html#type:"},
    {"string", "types.html#type:"},
    {"boolean", "types.html#type:"},
    {"date", "types.html#type:"},
    {"datetime", "types.html#type:"}
};

HelpPage::HelpPage(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::HelpPage)
{
    m_ui->setupUi(this);
    setObjectName("HelpPage");
    m_ui->browser->setSearchPaths({":/docs"});
    m_ui->browser->setSource(QUrl("index.html"));
    m_ui->btn_home->setIcon(icons::getIcon("home"));
    m_ui->btn_search->setIcon(icons::getIcon("search"));


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
            m_ui->browser->setSource(QUrl(QString("%2%1").arg(it.key().c_str(), it.value().c_str())));
        }
    }
}

