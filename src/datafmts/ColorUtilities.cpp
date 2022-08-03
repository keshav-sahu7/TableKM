#include "ColorUtilities.hpp"

#include <QPainter>


QIcon createIcon(const QColor &color)
{
    QPixmap pixmap(96,96);
    pixmap.fill(color);
    QPainter painter(&pixmap);
    QPen pen;
    pen.setWidth(4);
    pen.setColor(Qt::black);
    painter.drawRect(0,0,96,96);
    return QIcon(pixmap);
}
