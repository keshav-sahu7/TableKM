#include "MiscFunctions_.hpp"
#include "KDefines.h"

#include <QDate>

km::Variant toKVariant(const QString &str, km::DataType data_type)
{
    switch(data_type)
    {
    case km::DataType::INT32:
        return km::KInt32(str.toInt());
    case km::DataType::INT64:
        return km::KInt64(str.toLongLong());
    case km::DataType::FLOAT32:
        return km::KFloat32(str.toFloat());
    case km::DataType::FLOAT64:
        return km::KFloat64(str.toDouble());
    case km::DataType::STRING:
        return km::KString(str.toStdString());
    case km::DataType::BOOLEAN:
        return km::KBoolean(str.trimmed().toLower() == "true");
    case km::DataType::DATE:
    {
        QDate date = QDate::fromString(str,"dd/MM/yyyy");
        return to_kdate(date.year(),date.month(),date.day());
    }
    case km::DataType::DATE_TIME:
    {
        QDateTime dt = QDateTime::fromString(str,"dd/MM/yyyy hh:mm:ss");
        auto date = dt.date();
        auto time = dt.time();
        return to_kdateTime(date.year(),date.month(),date.day(), time.hour(), time.minute(), time.second());
    }
    }
    return km::Variant();
}

QString toQString(const km::Variant &var)
{
    QString str;
    switch(var.index())
    {
    case 0:
        str = QString::number(var.asInt32());
        break;
    case 1:
        str = QString::number(var.asInt64());
        break;
    case 2:
        str = QString::number(var.asFloat32());
        break;
    case 3:
        str = QString::number(var.asFloat64());
        break;
    case 4:
        str = K_SQ_STR(var.asString());
        break;
    case 5:
        str = var.asBoolean() ? "true" : "false";
        break;
    case 6:
    {
        km::KDate date = var.asDate();
        str = QDate(date.year,date.month,date.day).toString("dd/MM/yyyy");
        break;
    }
    case 7:
    {
        km::KDateTime dt = var.asDateTime();
        str = QDateTime(QDate(dt.date.year,dt.date.month,dt.date.day),QTime(dt.time.hour,dt.time.minute,dt.time.second)).toString("dd/MM/yyyy hh:mm:ss");
        break;
    }
    }
    return str;
}
