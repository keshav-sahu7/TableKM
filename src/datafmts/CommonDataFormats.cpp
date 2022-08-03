#include "CommonDataFormats.hpp"

using namespace km::tp;

namespace k2qt
{




////////////////////////////////////////////////////////////////
QString asInt32(const km::Variant &v)
{
    return QString::number(v.asInt32());
}

QString asHexadecimalI32(const km::Variant &v, const QString &prefix)
{
    return prefix + QString::number(v.asInt32(),16);
}

QString asHexadecimalUpperI32(const km::Variant &v, const QString &prefix)
{
    return prefix + QString::number(v.asInt32(),16).toUpper();
}

QString asMoneyI32(const km::Variant &v)
{
    return QLocale::system().toCurrencyString(v.asInt32());
}



///////////////////////////////////////////////////////
QString asInt64(const km::Variant &v)
{
    return QString::number(v.asInt64());
}

QString asHexadecimalI64(const km::Variant &v, const QString &prefix)
{
    return prefix + QString::number(v.asInt64(),16);
}

QString asHexadecimalUpperI64(const km::Variant &v, const QString &prefix)
{
    return prefix + QString::number(v.asInt64(),16).toUpper();
}

QString asMoneyI64(const km::Variant &v)
{
    return QLocale::system().toCurrencyString(v.asInt64());
}



///////////////////////////////////////////////////////////////////
QString asFloat32(const km::Variant &v, char format, int precision)
{
    return QString::number(v.asFloat32(),format,precision);
}

QString asMoneyF32(const km::Variant &v)
{
    return QLocale::system().toCurrencyString(v.asFloat32());
}


//////////////////////////////////////////////////////////////////
QString asFloat64(const km::Variant &v, char format, int precision)
{
    return QString::number(v.asFloat64(),format,precision);
}

QString asMoneyF64(const km::Variant &v)
{
    return QLocale::system().toCurrencyString(v.asFloat64());
}


//////////////////////////////////////////////////////////
QString asString(const km::Variant &v)
{
    return QString::fromStdString(v.asString());
}

QString asLowerCasetring(const km::Variant &v)
{
    return QString::fromStdString(v.asString()).toLower();
}

QString asUpperCaseString(const km::Variant &v)
{
    return QString::fromStdString(v.asString()).toUpper();
}


//////////////////////////////////////////////////////////////
QString asBoolean(const km::Variant &v, const QString &yes, const QString &no)
{
    return v.asBoolean()?yes:no;
}



///////////////////////////////////////////////////////////////
QString asDate(const km::Variant &v, QStringView format)
{
    const KDate &date = v.asDate();
    return QDate(date.year, date.month, date.day).toString(format);
}



///////////////////////////////////////////////////////////////
QString asDateTime (const km::Variant &v, QStringView format)
{
    const KDateTime &date_time = v.asDateTime();
    const auto &date = date_time.date;
    const auto &time = date_time.time;
    return QDateTime(QDate{date.year, date.month, date.day},QTime{time.hour, time.minute, time.second}).toString(format);
}











}
