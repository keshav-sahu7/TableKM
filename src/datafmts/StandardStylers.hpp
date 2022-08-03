#ifndef STANDARDSTYLERS_HPP
#define STANDARDSTYLERS_HPP

#include <array>

#include "CommonDataFormats.hpp"

namespace k2qt
{

using TextFormatter = std::function<QString(const km::Variant &)>;

QStringList getIntegerSFmts();
QStringList getFloatSFmts();
QStringList getStringSFmts();
QStringList getBooleanSFmts();
QStringList getDateSFmts();
QStringList getDateTimeSFmts();

QList<std::pair<QString,QString>> getBooleanSFmtsPairs();


QStringList getDateSFmtsAsCurrentDate();
QStringList getDateTimeSFmtsAsCurrentDateTime();

/**
 * Returns safe number less than max. If num >= max it returns 0.
 */
inline unsigned insideBoundary(unsigned num, unsigned max)
{
    return num < max ? num : 0;
}

/**
   * Currently a total of 6 integer formatters are available.
   * 0 or > 5 => as int
   * 1 => as hex lower
   * 2 => as hex upper
   * 3 => as hex lower with prefix "0x"
   * 4 => as hex upper with prefix "0x"
   * 5 => as money localized
   */
inline TextFormatter getStandardInt32Formatter(unsigned style_index)
{
    namespace ph = std::placeholders;
    std::array<TextFormatter,6> formatters = {
        &asInt32,      //0
        std::bind(&asHexadecimalI32, ph::_1, QString()), //1
        std::bind(&asHexadecimalUpperI32, ph::_1, QString()), //2
        std::bind(&asHexadecimalI32, ph::_1, "0x"), //3
        std::bind(&asHexadecimalUpperI32, ph::_1, "0x"), //4
        &asMoneyI32        //5
    };
    return formatters[insideBoundary(style_index,formatters.size())];
}




/**
   * same behaviour as getStandardInt32Formatter.
   */
inline TextFormatter getStandardInt64Formatter(unsigned style_index)
{
    namespace ph = std::placeholders;
    std::array<TextFormatter, 6> formatters = {
        &asInt64,      //0
        std::bind(&asHexadecimalI64, ph::_1, QString()), //1
        std::bind(&asHexadecimalUpperI64, ph::_1, QString()), //2
        std::bind(&asHexadecimalI64, ph::_1, "0x"), //3
        std::bind(&asHexadecimalUpperI64, ph::_1, "0x"), //4
        &asMoneyI64,       //5
    };
    return formatters[insideBoundary(style_index,formatters.size())];
}


/**
   *  Currently a total of 6 floating point formatters are available
   *  0 or > 4 => automatic lower     //0.001313 or 1.313e+3
   *  1 => automatic upper            //0.001313 or 1.313E+3
   *  2 => decimal                    //0.001313
   *  3 => scientific lower           //1.313e+3
   *  4 => scientific upper           //1.313E+3
   *  5 => as money                   //$ 10.00
   *  [Note : With a default precision of 6.]
   */
inline TextFormatter getStandardFloat32Formatter(unsigned style_index)
{
    if(style_index == 5)
        return asMoneyF32;
    return std::bind(&asFloat32, std::placeholders::_1, "gGfeE"[insideBoundary(style_index,5)], 6);
}


/**
   *  Same behaviour as getStandardFloat32Formatter.
   */
inline TextFormatter getStandardFloat64Formatter(unsigned style_index)
{
    if(style_index == 5)
        return asMoneyF64;
    return std::bind(&asFloat64, std::placeholders::_1, "gGfeE"[insideBoundary(style_index,5)], 6);
}


/**
   * Currently a total of 3 formats are supported
   * 0 or > 2 => original string
   * 1 => lower case string
   * 2 => upper case string
   */
inline TextFormatter getStandardStringFormatter(unsigned style_index)
{
    std::array<TextFormatter,3> formats = {&asString, &asLowerCasetring, &asUpperCaseString};
    return formats[insideBoundary(style_index,formats.size())];
}

/**
   * Currently a total of 6 standard format **TEXTS** are available
   * 0 or > 5 => True/False
   * 1 => true/false
   * 2 => TRUE/FALSE
   * 3 => Yes/No
   * 4 => yes/no
   * 5 => YES/NO
   */
inline TextFormatter getStandardBooleanFormatter(unsigned style_index)
{
    auto list = getBooleanSFmtsPairs();
    const auto &pattern = list[insideBoundary(style_index, list.size())];
    return std::bind(&asBoolean, std::placeholders::_1, pattern.first, pattern.second);
}


/**
   * Currently a total of 11 formats are supported.
   * 0       => localized long format
   * 1 => localized short format
   * 2 => dd/MM/yyyy      09/05/2022
   * 3 => dd-MM-yyyy      09-05-2022
   * 4 => dd.MM.yyyy      09.05.2022
   * 5 => MM/dd/yyyy      05/09/2022
   * 6 => MM-dd-yyyy      05-09-2022
   * 7 => MM.dd.yyyy      05.09.2022
   * 8 => ddd MMMM d yyyy       Mon May 9 2022
   * 9 => dddd MMMM d yyyy      Monday May 9 2022
   * 10 => MMM d yyyy          May 9 2022  (short month name)
   */
inline TextFormatter getStandardDateFormatter(unsigned style_index)
{
    auto list = getDateSFmts();
    return std::bind(&asDate, std::placeholders::_1, list[insideBoundary(style_index,list.size())]);
}


/**
   *    Currently a total of 11 formats are available.
   *    0 or > 10 => localized short form
   *    1 => localized long form
   *    2 => dd/MM/yyyy hh:mm ap
   *    3 => dd-MM-yyyy hh:mm ap
   *    4 => dd.MM.yyyy hh:mm ap
   *    5 => MM/dd/yyyy hh:mm ap
   *    6 => MM-dd-yyyy hh:mm ap
   *    7 => MM.dd.yyyy hh:mm ap
   *    8 => ddd MMMM d yyyy hh:mm:ss
   *    9 => dddd MMMM d yyyy hh:mm:ss
   *    10 => MMM d yyyy hh:mm
   */
inline TextFormatter getStandardDateTimeFormatter(unsigned style_index)
{
    auto list = getDateTimeSFmts();
    return std::bind(&asDateTime, std::placeholders::_1, list[insideBoundary(style_index,list.size())]);
}
}

#endif // STANDARDSTYLERS_HPP
