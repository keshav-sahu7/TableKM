#ifndef CELLSTYLER_HPP
#define CELLSTYLER_HPP

#include <QDialog>
#include <QSettings>
#include <functional>
#include <vector>

#include <QString>
#include <QColor>
#include <kmt/Column.hpp>
#include <kmt/Core.hpp>


#include "StandardStylers.hpp"

namespace k2qt
{

struct CellStyler
{
    TextFormatter format;
    Qt::AlignmentFlag hrzt_align;
    Qt::AlignmentFlag vrtc_align;
    QColor text_color;
    QColor bg_color;
    unsigned style_index = 0;
};


//get a formatter, if use_custom=true then settings must be a valid QSettings object.
TextFormatter getFormatter(km::DataType data_type, unsigned style_index);
CellStyler getCellStyler(QSettings *settings, const QString &unique_name, const km::ColumnMetaData &meta);
std::vector<CellStyler> getCellStylers(const QString &file, const QString& unique_name, const std::vector<km::ColumnMetaData> &column_vec);

void writeSettings(const CellStyler &styler, QSettings *settings, const km::ColumnMetaData &column, QString unique_name);
}
#endif // CELLSTYLER_HPP
