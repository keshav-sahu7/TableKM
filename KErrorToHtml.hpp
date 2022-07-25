#if 0
#ifndef KERRORTOHTML_HPP
#define KERRORTOHTML_HPP

#include <QString>


inline void formatToHtml(QString &err)
{
    // format the error string
    err.replace(QRegularExpression("^(.+)~"), QStringLiteral("<b>\\1</b>~"));
    err.replace(QRegularExpression("~(.+ Issue) :"), QStringLiteral("~<i><span style=\"color:#ff0000;\">\\1</span></i> :"));
    err.replace(QRegularExpression("`(([^`\\\\]|\\\\.)*)`"), QStringLiteral("<span style=\"color:#800080;\">`\\1`</span>"));
}

#endif // KERRORTOHTML_HPP
#endif
