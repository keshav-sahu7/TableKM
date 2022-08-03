#include <string>
#include <QString>

extern std::string placeQuotes(const std::string &str)
{
    QString qstr(QString::fromLatin1(str.c_str()));
    qstr.replace("&quot;","\"");
    qstr.replace("&\"","&quot;");
    return qstr.toStdString();
}

extern std::string replaceQuotes(const std::string &str)
{
    QString qstr(QString::fromLatin1(str.c_str()));
    qstr.replace("&quot;","&&quot;");
    qstr.replace("\"","&quot;");
    return qstr.toStdString();
}

