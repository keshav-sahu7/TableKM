#include "CellStyler.hpp"

#include <QApplication>
#include <QFile>
#include <QTableView>

#include <kmt/Column.hpp>

#include "CommonDataFormats.hpp"
#include "StandardStylers.hpp"
#include "ColorUtilities.hpp"
#include "../misc/KDefines.h"

#define IGNORE_LOCAL_VARIABLE [[maybe_unused]]


namespace k2qt
{

namespace{
//assign settings object with an opened group and it automatically closes it.
struct SettingsHelper_
{
    QSettings *m_settings;
    SettingsHelper_(QSettings *settings) : m_settings(settings) {}
    ~SettingsHelper_() { if(m_settings) m_settings->endGroup();}
};
}


//[horizontal,vertical]
static std::pair<Qt::AlignmentFlag, Qt::AlignmentFlag> getDefaultAlignment(km::DataType type)
{
    Qt::AlignmentFlag horizontal_alignments[] = {
        Qt::AlignRight, Qt::AlignRight,
        Qt::AlignLeft, Qt::AlignLeft,
        Qt::AlignHCenter, Qt::AlignHCenter,
        Qt::AlignLeft, Qt::AlignLeft
    };
    return {horizontal_alignments[km::indexForDataType(type,0)], Qt::AlignVCenter};
}


TextFormatter getFormatter(km::DataType data_type, unsigned style_index)
{
    namespace ph = std::placeholders;

    switch(data_type)
    {
    case km::DataType::INT32:
        return k2qt::getStandardInt32Formatter(style_index);

    case km::DataType::INT64:
        return k2qt::getStandardInt64Formatter(style_index);

    case km::DataType::FLOAT32:
    {
        if(style_index == 5)    //money format
            return k2qt::asMoneyF32;
        //else
        return std::bind(&k2qt::asFloat32, ph::_1,
                         "gGfeE"[k2qt::insideBoundary(style_index, 5)], 6);
    }

    case km::DataType::FLOAT64:
    {
        if(style_index == 5)    //money format
            return k2qt::asMoneyF64;
        return std::bind(&k2qt::asFloat64, ph::_1,
                         "gGfeE"[k2qt::insideBoundary(style_index, 5)], 6);
    }

    case km::DataType::STRING:
        return k2qt::getStandardStringFormatter(style_index);

    case km::DataType::BOOLEAN:
    {
        return k2qt::getStandardBooleanFormatter(style_index);
    }

    case km::DataType::DATE:
    {
        return k2qt::getStandardDateFormatter(style_index);
    }

    case km::DataType::DATE_TIME:
    {
        return k2qt::getStandardDateTimeFormatter(style_index);
    }

    }
    return nullptr;
}


CellStyler getCellStyler(QSettings *settings, const QString &unique_name, const km::ColumnMetaData &meta)
{
    CellStyler styler;
    QString settings_cell_name = QString("%1/%2").arg(unique_name).arg(meta.column_name.c_str());
    bool contains_settings = settings && settings->childGroups().contains(unique_name);

    auto alignments = getDefaultAlignment(meta.data_type);

    styler.text_color = QTableView().palette().color(QPalette::Text);
    styler.bg_color = QTableView().palette().color(QPalette::Base);
    styler.hrzt_align = alignments.first;
    styler.vrtc_align = alignments.second;
    styler.style_index = 0;

    if(contains_settings)      //if settings exists overwrite
    {
        settings->beginGroup(settings_cell_name);
        styler.text_color = settings->value("text-color", QVariant::fromValue(styler.text_color)).value<QColor>();
        styler.bg_color = settings->value("background-color", QVariant::fromValue(styler.bg_color)).value<QColor>();
        styler.hrzt_align = settings->value("horizontal-alignment", styler.hrzt_align).value<Qt::AlignmentFlag>();
        styler.vrtc_align = settings->value("vertical-aligment", styler.vrtc_align).value<Qt::AlignmentFlag>();
        styler.style_index = settings->value("display-style" , 0).toUInt();
        settings->endGroup();
    }
    styler.format = getFormatter(meta.data_type, styler.style_index);
    return styler;
}

std::vector<CellStyler> getCellStylers(const QString &file, const QString& unique_name, const std::vector<km::ColumnMetaData> &column_vec)
{
    QSettings settings(file, QSettings::IniFormat);
    std::vector<CellStyler> stylers;
    stylers.reserve(column_vec.size());
    for(const auto &column : column_vec)
        stylers.push_back(getCellStyler(&settings,unique_name,column));
    return stylers;
}

void writeSettings(const CellStyler &styler, QSettings *settings, const km::ColumnMetaData &column, QString unique_name)
{
    settings->beginGroup(QString("%1/%2").arg(unique_name).arg(column.column_name.c_str()));
    settings->setValue("text-color", QVariant::fromValue(styler.text_color));
    settings->setValue("background-color", QVariant::fromValue(styler.bg_color));
    settings->setValue("display-style", styler.style_index);
    settings->setValue("horizontal-alignment", styler.hrzt_align);
    settings->setValue("vertical-alignment", styler.vrtc_align);
    settings->endGroup();
}

}
