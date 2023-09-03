#ifndef MISCFUNCTIONS_HPP
#define MISCFUNCTIONS_HPP
#include <QString>
#include <kmt/Core.hpp>

inline km::KDate to_kdate(uint16_t year, uint8_t month, uint8_t day)
{
    return km::KDate{year,month,day};
}

inline km::KDateTime to_kdateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    return km::KDateTime{{year,month,day},{hour,minute,second}};
}

km::Variant toKVariant(const QString &str, km::DataType data_type);
QString toQString(const km::Variant &var);

#endif // MISCFUNCTIONS_HPP
