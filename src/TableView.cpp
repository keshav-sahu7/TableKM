#include "TableView.hpp"

#include <QHeaderView>
#include <QMenu>

#include <QDebug>

#include "K2QtTableModel.hpp"
#include "ThemeHandler2.hpp"
#include "datafmts/CommonDataFormats.hpp"
#include "datafmts/ColorUtilities.hpp"

#include "delegates/Delegates.hpp"
#include "misc/KDefines.h"


QList<std::pair<QString,QColor>> TableView::colors = {
    {"black",Qt::black},
    {"white", Qt::white},
    {"dark gray", Qt::darkGray},
    {"gray", Qt::gray},
    {"light gray", Qt::lightGray},
    {"red", Qt::red},
    {"green", Qt::green},
    {"blue", Qt::blue},
    {"cyan", Qt::cyan},
    {"magenta", Qt::magenta},
    {"yellow", Qt::yellow},
    {"dark red", Qt::darkRed},
    {"dark green", Qt::darkGreen},
    {"dark blue", Qt::darkBlue},
    {"dark cyan", Qt::darkCyan},
    {"dark magenta", Qt::darkMagenta},
    {"dark yellow", Qt::darkYellow}
};

TableView::TableView(QWidget *parent)
    : QTableView(parent),
      m_model(nullptr)
{
    QHeaderView *horizontal_header = this->horizontalHeader();
    horizontal_header->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(horizontal_header,&QHeaderView::customContextMenuRequested, this, &TableView::popupContextMenu);
}


//this function is called from QHeaderView (horizontal) , so point is relative to QHeaderView
void TableView::popupContextMenu(const QPoint &point)
{
    QHeaderView *horizontal_header = this->horizontalHeader();
    int column_index = horizontal_header->logicalIndexAt(point);
    auto mdata = getK2QtModel()->getColumnMetaData(column_index);
    K_RETURN_IF_NOT(mdata);
    QStringList data_formats = getDisplayFormats(mdata.value().data_type);

    QMenu menu;
    if(!data_formats.isEmpty())
    {
        QMenu *menu_formats = menu.addMenu(tr("Show As"));
        for(int i = 0, size = data_formats.size(); i < size; ++i)
        {
            QAction *action = menu_formats->addAction(data_formats[i]);
            action->setProperty("column-index",column_index);
            action->setProperty("column-format",i);
            connect(action,&QAction::triggered, this, &TableView::formatChangeRequested);
        }
    }

    QMenu *menu_alignments = menu.addMenu(tr("Alignment"));

    QMenu *menu_h_aligns = menu_alignments->addMenu(tr("Horizontal"));
    std::pair<Qt::AlignmentFlag, QString> h_aligns[3] = {
        {Qt::AlignLeft, tr("Left")}, {Qt::AlignHCenter, tr("Center")}, {Qt::AlignRight, tr("Right")}
    };

    for(const auto &align : h_aligns)
    {
        QAction *action = menu_h_aligns->addAction(align.second);
        action->setProperty("column-index",column_index);
        action->setProperty("column-halignment",align.first);
        connect(action,&QAction::triggered, this, &TableView::alignmentChangeRequested);
    }

    QMenu *menu_v_aligns = menu_alignments->addMenu(tr("Vertical"));
    std::pair<Qt::AlignmentFlag, QString> v_aligns[3] = {
        {Qt::AlignTop, "Top"},{Qt::AlignVCenter, "Center"},{Qt::AlignBottom, "Bottom"}
    };

    for(const auto &align : v_aligns)
    {
        QAction *action = menu_v_aligns->addAction(align.second);
        action->setProperty("column-index",column_index);
        action->setProperty("column-valignment",align.first);
        connect(action,&QAction::triggered, this, &TableView::alignmentChangeRequested);
    }

    QMenu *menu_fore = menu.addMenu("Foreground Color");
    for(const auto &color_p : TableView::colors)
    {
        QAction *action = menu_fore->addAction(createIcon(color_p.second),color_p.first);
        action->setProperty("column-index",column_index);
        action->setProperty("foreground-color", QVariant::fromValue(color_p.second));
        connect(action,&QAction::triggered, this, &TableView::foregroundColorChangeRequested);
    }

    QMenu *menu_back = menu.addMenu("Background Color");
    for(const auto &color_p : TableView::colors)
    {
        QAction *action = menu_back->addAction(createIcon(color_p.second),color_p.first);
        action->setProperty("column-index",column_index);
        action->setProperty("background-color", QVariant::fromValue(color_p.second));
        connect(action,&QAction::triggered, this, &TableView::backgroundColorChangeRequested);
    }

    if(getK2QtModel()->getTableType() == TableType::BASIC_VIEW){
        bool key_column = (getK2QtModel()->getAbsTable()->getKeyColumn() == static_cast<km::IndexType>(column_index));
        bool ascending_order = getK2QtModel()->getAbsTable()->getSortingOrder() == km::SortingOrder::ASCENDING;

        QMenu *s_menu = menu.addMenu(tr("Sort Using This Column"));
        if((key_column && !ascending_order) || !key_column)
        {
            QAction *action = s_menu->addAction(icons::getIcon("ascending"),tr("Ascending"));
            action->setProperty("column-index",column_index);
            action->setProperty("sorting-order", 0 /*ascending*/);
            connect(action, &QAction::triggered, this, &TableView::sortViewModel);
        }
        if((key_column && ascending_order) || !key_column)
        {
            QAction *action = s_menu->addAction(icons::getIcon("descending"),tr("Descending"));
            action->setProperty("column-index",column_index);
            action->setProperty("sorting-order", 1 /*descending*/);
            connect(action, &QAction::triggered, this, &TableView::sortViewModel);
        }
    }
    menu.exec(horizontal_header->mapToGlobal(point));

}

void TableView::formatChangeRequested()
{
    QAction *action = dynamic_cast<QAction*>(sender());
    m_model->setDataFormat(action->property("column-index").toInt(),action->property("column-format").toInt());
}

void TableView::alignmentChangeRequested()
{
    QAction *action = dynamic_cast<QAction*>(sender());
    int column_index = action->property("column-index").toInt();
    const k2qt::CellStyler &styler = m_model->getStyler(column_index);
    QVariant h_alignment = action->property("column-halignment");

    if(h_alignment.isValid())
        m_model->setAlignment(column_index, h_alignment.value<Qt::AlignmentFlag>(), styler.vrtc_align);
    else
        m_model->setAlignment(column_index, styler.hrzt_align, action->property("column-valignment").value<Qt::AlignmentFlag>());
}


void TableView::sortViewModel()
{
    QAction *action = dynamic_cast<QAction*>(sender());
    K_RETURN_IF_NOT(action);
    km::SortingOrder s_order = static_cast<km::SortingOrder>(action->property("sorting-order").toInt());
    km::IndexType column_index = static_cast<km::IndexType>(action->property("column-index").toInt());

    if(auto view = dynamic_cast<km::AbstractView*>(getK2QtModel()->getAbsTable()))
    {
        auto clm = view->columnAt(column_index);
        K_RETURN_IF_NOT(clm);
        view->sortBy(clm.value().first,s_order);
    }
}

void TableView::foregroundColorChangeRequested()
{
    QAction *action = dynamic_cast<QAction*>(sender());
    K_RETURN_IF_NOT(action);
    km::IndexType column_index = static_cast<km::IndexType>(action->property("column-index").toInt());
    QColor color = action->property("foreground-color").value<QColor>();
    getK2QtModel()->setForegroundColor(color,column_index);
}

void TableView::backgroundColorChangeRequested()
{
    QAction *action = dynamic_cast<QAction*>(sender());
    K_RETURN_IF_NOT(action);
    km::IndexType column_index = static_cast<km::IndexType>(action->property("column-index").toInt());
    QColor color = action->property("background-color").value<QColor>();
    getK2QtModel()->setBackgroundColor(color,column_index);
}

QStringList TableView::getDisplayFormats(km::DataType data_type)
{
    using dt = km::DataType;

    QStringList display_formats;
    switch(data_type)
    {
    case dt::INT32:
    case dt::INT64:
        display_formats = k2qt::getIntegerSFmts();
        break;
    case dt::FLOAT32:
    case dt::FLOAT64:
        display_formats = k2qt::getFloatSFmts();
        break;
    case dt::STRING:
        display_formats = k2qt::getStringSFmts();
        break;
    case dt::BOOLEAN:
        display_formats = k2qt::getBooleanSFmts();
        break;
    case dt::DATE:
        display_formats = k2qt::getDateSFmtsAsCurrentDate();
        break;
    case dt::DATE_TIME:
        display_formats = k2qt::getDateTimeSFmtsAsCurrentDateTime();
        break;
    default:
        break;
    }
    return display_formats;
}

void TableView::setModel(QAbstractItemModel *model)
{
    if(model == nullptr)
    {
        if(m_model) m_model->setViewWidget(nullptr);
        m_model = nullptr;
        QTableView::setModel(nullptr);
        return;
    }

    auto model_ = dynamic_cast<K2QtTableModel*>(model);
    K_RETURN_IF(!model_);

    if(model_->isOpened()){
        qWarning() << "TableView : K2QtModel `" << model_->getName() << "` is already opened!"
                   << "Not setting this model to this widget.";
        return;
    }

    if(m_model)
    {
        m_model->setViewWidget(nullptr);
    }

    m_model = model_;
    QTableView::setModel(model);


    std::vector<std::pair<std::string,km::DataType>> col_info = m_model->getColumnInfo();
    for(int i = 0, count = col_info.size(); i < count; ++i)
    {
        switch(col_info[i].second)
        {
        case km::DataType::INT32:
            setItemDelegateForColumn(i, getInt32Delegate(this));
            break;
        case km::DataType::INT64:
            setItemDelegateForColumn(i, getInt64Delegate(this));
            break;
        case km::DataType::FLOAT32:
            setItemDelegateForColumn(i, getFloat32Delegate(this));
            break;
        case km::DataType::FLOAT64:
            setItemDelegateForColumn(i, getFloat64Delegate(this));
            break;
        case km::DataType::DATE:
            setItemDelegateForColumn(i, new DateDelegate(this));
            break;
        case km::DataType::DATE_TIME:
            setItemDelegateForColumn(i, new DateTimeDelegate(this));
            break;
        default:
            break;
        }
    }
    m_model->setViewWidget(this);
    //manually emit the signal
    emit model_->modifiedStatusChanged(model_->isModified());

}

TableView::~TableView()
{
    if(m_model)
        m_model->setViewWidget(nullptr);
}



