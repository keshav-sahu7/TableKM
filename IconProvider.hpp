#ifndef ICONPROVIDER_HPP
#define ICONPROVIDER_HPP

#include <cstring>

#include <QIcon>
#include <QStringView>
#include <QStringBuilder>


namespace icons{

namespace internal_{
extern char theme_path[7];   //"light\" (default) | "dark\"
}
/**
 * @brief setTheme sets theme name for icons
 * @note if theme_name is "dark", it will set theme_path to "dark/"
 * else it will set "light/".
 */
void setThemePath(const char* theme_name);

//returns QIcon(":/icons/arrow_left.png") for arrow_left
inline QIcon getIcon(const char *icon_name){
    return QIcon(QStringLiteral(":/icons/") % icon_name % ".png");
}

//returns QIcon(":/icons/light/arrow_left.png") for arrow_left (when theme is set to light)
inline QIcon getThemedIcon(const char *icon_name)
{
    //as there is no QStringView constructor for ascii characters, QStringLiteral is used instead.
    return QIcon(QStringLiteral(":/icons/") % internal_::theme_path % icon_name % ".png");
}

}
#endif // ICONPROVIDER_HPP
