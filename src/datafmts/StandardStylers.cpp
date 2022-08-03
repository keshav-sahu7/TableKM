#include "StandardStylers.hpp"


namespace k2qt
{



QStringList getIntegerSFmts()
{
    QStringList list(
                {"Decimal (Base 10)",
                 "Hex (Base 16) Lower",
                 "Hex (Base 16) Upper",
                 "Hex Lower With Prefix",
                 "Hex Upper With Prefix",
                 "As Money"
                });
    return list;
}

QStringList getFloatSFmts()
{
    QStringList list(
                {"Automatic (Lower)",
                 "Automatic (Upper)",
                 "Decimal",
                 "Scientific (Lower)",
                 "Scientific (Upper)"
                });
    return list;
}

QStringList getStringSFmts()
{
    QStringList list(
                {"Original",
                 "Lower Case",
                 "Upper Case"
                });
    return list;
}

QStringList getBooleanSFmts()
{
    QStringList list(
                {"True && False",
                 "true && false",
                 "TRUE && FALSE",
                 "Yes && No",
                 "yes && no",
                 "YES && NO"
                });
    return list;
}

QStringList getDateSFmts()
{
    QStringList list(
                {QLocale::system().dateFormat(QLocale::ShortFormat),
                 QLocale::system().dateFormat(QLocale::LongFormat),
                 "dd/MM/yyyy",
                 "dd-MM-yyyy",
                 "dd.MM.yyyy",
                 "MM/dd/yyyy",
                 "MM-dd-yyyy",
                 "MM.dd.yyyy",
                 "ddd MMMM d yyyy",
                 "dddd MMMM d yyyy",
                 "MMM d yyyy"
                });
    return list;
}

QStringList getDateTimeSFmts()
{
    QStringList list(
                {QLocale::system().dateTimeFormat(QLocale::ShortFormat),
                 QLocale::system().dateTimeFormat(QLocale::LongFormat),
                 "dd/MM/yyyy hh:mm ap",
                 "dd-MM-yyyy hh:mm ap",
                 "dd.MM.yyyy hh:mm ap",
                 "MM/dd/yyyy hh:mm ap",
                 "MM-dd-yyyy hh:mm ap",
                 "MM.dd.yyyy hh:mm ap",
                 "ddd MMMM d yyyy hh:mm:ss",
                 "dddd MMMM d yyyy hh:mm:ss",
                 "MMM d yyyy hh:mm"
                });
    return list;
}

QList<std::pair<QString, QString> > getBooleanSFmtsPairs()
{
    QList<std::pair<QString,QString>> list(
                {{"True","False"},
                 {"true","false"},
                 {"TRUE","FALSE"},
                 {"Yes","No"},
                 {"yes","no"},
                 {"YES","NO"}
                });
    return list;
}

QStringList getDateSFmtsAsCurrentDate()
{
    QDate date = QDate::currentDate();
    QStringList list;
    for(const auto &date_format : getDateSFmts())
    {
        list.append(date.toString(date_format));
    }
    return list;
}

QStringList getDateTimeSFmtsAsCurrentDateTime()
{
    QDateTime date_time = QDateTime::currentDateTime();
    QStringList list;
    for(const auto &date_format : getDateTimeSFmts())
    {
        list.append(date_time.toString(date_format));
    }
    return list;
}



}
