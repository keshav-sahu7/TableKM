#include "ThemeHandler2.hpp"

#include <QApplication>
#include <QFile>
#include <QPalette>
#include <QStringView>
#include <QStyle>

#include "misc/KDefines.h"

inline namespace {

inline void setGlobalStyleSheet(const QString &stylesheet)
{
    QApplication::style()->unpolish(qApp);
    qApp->setStyleSheet(stylesheet);
    QApplication::style()->polish(qApp);
}

/* UB with empty str*/
inline QString toSentenseCase(const QString& str)
{
    QString rt(str[0].toUpper());
    rt += str.last(str.length() - 1);
    return rt;
}

inline QString colorFormat(const QColor &color)
{
    return QString("%1, %2, %3").arg(color.red()).arg(color.green()).arg(color.blue());
}

} // end of empty namespace





Theme::Theme()
    : name( QString::fromLatin1( "default")),
    background_color( QApplication::palette().window().color()),
    base_color( QApplication::palette().base().color())
{

}


Theme &Theme::getInstance()
{
    static Theme instance;
    return instance;
}

void Theme::setTheme(const QString &theme, const QString &flavour)
{
    Theme &instance = getInstance();

    QString theme_contents;

    if(theme == "dark" || theme == "light")
    {
        QFile file(QString(":/%1%2.qss").arg(toSentenseCase(theme), toSentenseCase(flavour)));
        if(file.open(QFile::ReadOnly))
        {
            theme_contents = file.readAll();
            file.close();
        }
        else return;

        auto flavour_lwr = flavour.toLower();

        if(flavour_lwr == "maroon"){
            instance.flavour_color = QColor::fromRgb(195, 33, 72);
            instance.hover_color = QColor::fromRgb(171, 29, 63);
        }
        else if(flavour_lwr == "purple"){
            instance.flavour_color = QColor::fromRgb(121, 80, 245);
            instance.hover_color = QColor::fromRgb( 89, 38, 242);
        }
        else if(flavour_lwr == "blue"){
            instance.flavour_color = QColor::fromRgb(34, 34, 255);
            instance.hover_color = QColor::fromRgb( 0, 0, 223);
        }
        else if(flavour_lwr == "green"){
            instance.flavour_color = QColor::fromRgb(31, 163, 70);
            instance.hover_color = QColor::fromRgb(24, 124, 54);
        }
        else return;

        instance.name = theme;
        instance.flavour = flavour;
        instance.background_color = QColor::fromRgb(59, 59, 59);
        instance.base_color = (theme == "dark")? QColor::fromRgb(36,36,36) : QColor(Qt::white);
        instance.path = ":/icons/" + instance.flavour;
    }

    QFile file(":/CustomStyleSheet.qss");
    if(file.open(QFile::ReadOnly))
    {
        auto custom_contents = QString::fromLatin1(file.readAll());

        custom_contents.replace("#BASE_COLOR", colorFormat(instance.base_color))
            .replace("#WINDOW_COLOR", colorFormat(instance.background_color))
            .replace("#FLAVOUR_COLOR", colorFormat(instance.flavour_color))
            .replace("#HOVER_COLOR", colorFormat(instance.hover_color));

        theme_contents += custom_contents;
    }

    setGlobalStyleSheet(theme_contents);
}


