#ifndef COMMONDATAFORMATS_HPP
#define COMMONDATAFORMATS_HPP
#include <QVariant>
#include <QDate>
#include <QDateTime>
#include <kmt/Core.hpp>


/**
 *
*/

namespace k2qt{


QString asInt32(const km::Variant &v);
QString asMoneyI32(const km::Variant &v);
QString asHexadecimalI32(const km::Variant &v, const QString &prefix);
QString asHexadecimalUpperI32(const km::Variant &v, const QString &prefix);


QString asInt64(const km::Variant &v);
QString asMoneyI64(const km::Variant &v);
QString asHexadecimalI64(const km::Variant &v, const QString &prefix);
QString asHexadecimalUpperI64(const km::Variant &v, const QString &prefix);

QString asFloat32(const km::Variant &v, char format = 'g', int precision = 6);
QString asMoneyF32(const km::Variant &v);

QString asFloat64(const km::Variant &v, char format = 'g', int precision = 6);
QString asMoneyF64(const km::Variant &v);

QString asString(const km::Variant &v);
QString asLowerCasetring(const km::Variant &v);
QString asUpperCaseString(const km::Variant &v);

QString asBoolean(const km::Variant &v, const QString &yes = "True", const QString &no = "False");

QString asDate(const km::Variant &v, QStringView format);
QString asDateTime (const km::Variant &v, QStringView format);

}   //namespace cf

#endif // COMMONDATAFORMATS_HPP
