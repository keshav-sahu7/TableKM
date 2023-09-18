#ifndef THEMEHANDLER_H
#define THEMEHANDLER_H

#include <cstring>

#include <QIcon>
#include <QStringView>
#include <QStringBuilder>
#include <QDebug>

class Theme
{
public:
    static Theme& getInstance();
    static const QString &getThemeName();
    static const QString &getThemePath();
    static const QString &getThemeFlavour();
    static QColor getFlavourColor();
    static QColor getHoverColor();
    static QColor getGeneralBackgroundColor();
    static QColor getBaseColor();
    static void setTheme(const QString &theme, const QString &flavour);

private:
    Theme();

    // path of the icons
    QString path;
    // name of the theme light|dark|default, default == no theme.
    QString name;
    // the flavour color of the theme like blue if theme is dark blue or light blue.
    QString flavour;
    // background color of the theme default : default-color / dark : rgb( 59, 59, 59)
    QColor background_color;
    // base color refers to color used to draw background of line edits
    QColor base_color;
    // color the theme uses for styling
    QColor flavour_color;
    // color used when mouse hover happens (only for dark/light themes)
    QColor hover_color;
};

inline const QString &Theme::getThemeName()
{
    return getInstance().name;
}

inline const QString &Theme::getThemePath()
{
    return getInstance().path;
}

inline const QString &Theme::getThemeFlavour()
{
    return getInstance().flavour;
}

inline QColor Theme::getFlavourColor()
{
    return getInstance().flavour_color;
}

inline QColor Theme::getHoverColor()
{
    return getInstance().hover_color;
}

inline QColor Theme::getGeneralBackgroundColor()
{
    return getInstance().background_color;
}

inline QColor Theme::getBaseColor()
{
    return getInstance().base_color;
}









namespace icons{


//returns QIcon(":/icons/light/arrow_left.png") for arrow_left (when theme is set to light)
inline QIcon getIcon(const char *icon_name)
{
    auto iconPath = QString("%1/%2.png").arg(Theme::getThemePath(), icon_name);
    return QIcon(iconPath);
}

} // end of namespace icons



#endif // THEMEHANDLER_H
