#ifndef COLOR_UTILITIES_HPP
#define COLOR_UTILITIES_HPP
#include <QIcon>
#include <QColor>
#include <QPixmap>
#include <QStringView>

QIcon createIcon(const QColor &color);

//return #argb format
inline QString toString(const QColor &color)
{
    return color.name(QColor::HexArgb);
}


#endif // COLOR_UTILITIES_HPP
