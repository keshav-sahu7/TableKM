#include "ThemeHandler.hpp"
#include <QDebug>


namespace theme
{

QString _theme_type;
QString _theme_name;
QString _theme_path;

QString ThemeTypeAsString(ThemeType theme_type);
QString ThemeNameAsString(ThemeName theme_name);

void setTheme(ThemeType theme_type, ThemeName theme_name)
{
    _theme_type = ThemeTypeAsString(theme_type);
    _theme_name = ThemeNameAsString(theme_name);
    _theme_path = QString(":/icons/%1/%2/").arg(_theme_type, _theme_name);
}


QString ThemeTypeAsString(ThemeType theme_type)
{
    switch(theme_type)
    {
    case theme::LIGHT:
        return "light";
    case theme::DARK:
        return "dark";
    default:
        qDebug() << "unknown theme type encountered : " << (int)theme_type;
        return "unknown";
    }
}

QString ThemeNameAsString(ThemeName theme_name)
{
    switch(theme_name)
    {
    case theme::GREEN:
        return "green";
    case theme::PURPLE:
        return "purple";
    case theme::BLUE:
        return "blue";
    case theme::MAROON:
        return "maroon";
    default:
        qDebug() << "unknown theme name encountered : " << (int)theme_name;
        return "unknown";
    }
}

}

