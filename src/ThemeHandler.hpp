#ifndef THEMEHANDLER_HPP
#define THEMEHANDLER_HPP

#include <QIcon>
#include "ThemeConsts.hpp"


namespace theme
{

inline QString getThemeType()
{
    extern QString _theme_type;
    return _theme_type;
}

inline QString getThemeName()
{
    extern QString _theme_name;
    return _theme_name;
}

inline QString getThemePath()
{
    extern QString _theme_path;
    return _theme_path;
}

void setTheme(ThemeType theme_type, ThemeName theme_name);

namespace icons
{

inline QIcon getIcon(const char* name)
{
    return QIcon(QString("%1/%2.png").arg(getThemePath(), name));
}

}
}

namespace icons = theme::icons;

#endif // THEMEHANDLER_HPP
