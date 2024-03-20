#include "K2QtTableModel.hpp"

#include <QDate>
#include <QDir>
#include <QStringBuilder>
#include <QTimer>
#include <QApplication>

#include <kmt/Table.hpp>
#include <kmt/BasicView.hpp>
#include <kmt/Core.hpp>
#include <kmt/TableIO.hpp>


#include "TableView.hpp"
#include "CustomErrMsgBox.hpp"
#include "DummyBasicView.hpp"
#include "rwview/ViewReadWrite.hpp"
#include "IconProvider.hpp"
#include "misc/KDefines.h"
#include "misc/SwitchLogHandlerHelper_.hpp"

K2QtTableModel::K2QtTableModel(km::AbstractTable *table, const QString &path, TableType table_type, QObject *parent)
    :
      QAbstractTableModel(parent),
      m_table(std::unique_ptr<km::AbstractTable>(table)),
      m_path(path),
      m_ttype(table_type),
      m_guiview(nullptr),
      m_modified(false)
{
    setObjectName("K2QtTableModel");
    km::IndexType column_count = table->columnCount();
    std::vector<km::ColumnMetaData> columns;
    columns.reserve(column_count);
    for(km::IndexType i = 0; i < column_count; ++i)
        columns.push_back(table->getColumnMetaData(i));
    m_stylers = k2qt::getCellStylers(path % "/settings.ini", getUniqueName(), columns);
}

K2QtTableModel::K2QtTableModel(km::Table *table, const QString &path, QObject *parent)
    : K2QtTableModel(table, path, TableType::TABLE, parent)
{
    if(table->rowCount() == 0)      //if newly created!
        saveTable(/*forced = */true);
}

K2QtTableModel::K2QtTableModel(km::BasicView *table, const QString &path, K2QtTableModel *parent)
    : K2QtTableModel(table, path, TableType::BASIC_VIEW, parent)
{
    ViewChangeNotifier *notifier = new ViewChangeNotifier(table,this);
    connect(notifier, &ViewChangeNotifier::kDataChanged, this, [this](auto r, auto c){
        emit dataChanged(index(r,c),index(r,c));
    });
    connect(notifier, &ViewChangeNotifier::kRowInserted, this, [this](auto r){
        beginInsertRows(QModelIndex(),r,r);
        endInsertRows();
    });
    connect(notifier, &ViewChangeNotifier::kRowDropped, this, [this](auto r){
        beginRemoveRows(QModelIndex(),r,r);
        endRemoveRows();
    });
    connect(notifier, &ViewChangeNotifier::kCompleteUpdateNeeded,this, [this](){
        beginResetModel();
        endResetModel();
    });
    connect(notifier, &ViewChangeNotifier::kColumnTransformed, this, [this](auto c){
        emit dataChanged(index(0,c),index(rowCount(QModelIndex()),c));
    });
}

K2QtTableModel::~K2QtTableModel()
{
    // disable this signal before calling saveTable();
    disconnect(this, &K2QtTableModel::modifiedStatusChanged, nullptr, nullptr);
    saveTable();
    auto list = findChildren<K2QtTableModel*>("K2QtTableModel", Qt::FindDirectChildrenOnly);
    for(auto l : list)
    {
        delete l;
    }
    if(isOpened()){
        getViewWidget()->deleteLater();
    }
}

QModelIndex K2QtTableModel::index(int row, int column, [[maybe_unused]] const QModelIndex &parent) const
{
    return createIndex(row,column);
}

int K2QtTableModel::rowCount([[maybe_unused]] const QModelIndex &parent) const
{
    return m_table->rowCount();
}

int K2QtTableModel::columnCount([[maybe_unused]] const QModelIndex &parent) const
{
    return m_table->columnCount();
}


QVariant K2QtTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    auto result = m_table->getData(index.row(),index.column());
    if(result){
        const k2qt::CellStyler &styler = m_stylers[index.column()];
        switch(role)
        {
        case Qt::DisplayRole:
            return styler.format(result.value());
        case Qt::TextAlignmentRole:
            return QVariant::fromValue(styler.hrzt_align | styler.vrtc_align);
        case Qt::ForegroundRole:
            return styler.text_color;
        case Qt::BackgroundRole:
            return styler.bg_color;
        case Qt::EditRole:
            return qtVariantFrom(result.value());
        }
    }
    return QVariant();
}

QVariant K2QtTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole && role != Qt::DecorationRole)
        return QVariant();

    if(orientation == Qt::Orientation::Horizontal && static_cast<km::IndexType>(section) < m_table->columnCount())
    {
        if(role == Qt::DisplayRole)
            return K_SQ_STR(m_table->columnAt(section).value().first);
        else if(getAbsTable()->getKeyColumn() == km::IndexType(section))
            return getAbsTable()->getSortingOrder() == km::SortingOrder::ASCENDING ? icons::getIcon("ascending") : icons::getIcon("descending");
    }
    else if(orientation == Qt::Orientation::Vertical && static_cast<km::IndexType>(section) < m_table->rowCount())
    {
        return section+1;
    }
    return QVariant();
}

Qt::ItemFlags K2QtTableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid()) return Qt::NoItemFlags;
    Qt::ItemFlags flag = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
    if(getTableType() == TableType::TABLE && index.column() != 0)
        return flag | Qt::ItemIsEditable;
    else
        return flag;
}

bool K2QtTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid() || role != Qt::EditRole)
        return false;
    m_table->setData(index.row(),index.column(), kmVariantFrom(value,m_table->getColumnMetaData(index.column()).data_type));
    dataChanged(index,index);
    setModifiedStatus(true);
    return true;
}

std::vector<std::pair<std::string, km::DataType>> K2QtTableModel::getColumnInfo() const
{
    std::vector<std::pair<std::string, km::DataType>> list;
    for(km::IndexType i = 0, count = m_table->columnCount(); i < count; ++i)
    {
        const km::ColumnMetaData &meta_data = m_table->getColumnMetaData(i);
        list.push_back({meta_data.display_name,meta_data.data_type});
    }
    return list;
}

std::optional<km::ColumnMetaData> K2QtTableModel::getColumnMetaData(int column_index) const
{
    if(column_index >= 0 && column_index < columnCount(QModelIndex()))
        return m_table->getColumnMetaData(column_index);
    return {};
}

void K2QtTableModel::saveTable(bool forced)
{
    if(getTableType() == TableType::TABLE)
    {
        if(forced || isModified())
            km::writeTableTo(dynamic_cast<km::Table*>(getAbsTable()), getPath().toStdString());
        std::string view_file = (getPath() + "/Views.xml").toStdString();
        saveViews(getAbsTable(), view_file.c_str());
        writeSettings();
        setModifiedStatus(false);
    }
}

QString K2QtTableModel::getUniqueName() const
{
    QString s;
    auto t = getAbsTable();
    while(t)
    {
        s.prepend(K_SQ_STR(t->getName()));
        auto v = dynamic_cast<const km::AbstractView*>(t);
        if(v)
        {
            s.prepend('-');
            t = v->getSourceTable();
        }
        else
        {
            t = nullptr;
        }
    }
    return s;
}





void K2QtTableModel::addRow(const std::vector<km::Variant> &elements)
{
    if(m_ttype != TableType::TABLE){
        qDebug("Warning : Insertion of a row in a View.");
        return;
    }
    km::Table *table = dynamic_cast<km::Table*>(m_table.get());
    auto index = table->insertRow(elements);
    if(index != km::INVALID_INDEX)
    {
        beginInsertRows(QModelIndex(),index,index);     //we don't know the exact location before insertion.
        endInsertRows();
        setModifiedStatus(true);
    }
}

void K2QtTableModel::dropRow(km::IndexType index)
{
    if(getTableType() != TableType::TABLE){
        qDebug("Warning : Dropping a row in a View.");
        return;
    }
    km::Table *table = dynamic_cast<km::Table*>(getAbsTable());
    beginRemoveRows(QModelIndex(),index,index);
    bool b = table->dropRow(index);
    endRemoveRows();
    if(b)
        setModifiedStatus(true);
}

void K2QtTableModel::addColumnFV(const km::ColumnMetaData &column_meta, const km::Variant &fill_with)
{
    if(getTableType() != TableType::TABLE){
        qDebug("Warning : Insertion of a column in a View.");
        return;
    }
    beginInsertColumns(QModelIndex(),columnCount(),columnCount());

    km::Table *table = dynamic_cast<km::Table*>(getAbsTable());

    QString error;
    SwitchLogHandlerHelper_ hdlr_locker([&error](const std::string &str){
        error = K_SQ_STR(str);
    });
    bool b = table->addColumn(column_meta,fill_with);
    endInsertColumns();
    if(b)
    {
        m_stylers.push_back(k2qt::getCellStyler(nullptr,getUniqueName(),column_meta));
        setModifiedStatus(true);
        emit newColumnAdded(columnCount()-1,column_meta, 0, fill_with, QString());
    }
    else
    {
        beginRemoveColumns(QModelIndex(), columnCount(), columnCount());
        endRemoveColumns();
        CustomErrMsgBox::show(nullptr,tr("Add Column Error"),error);
    }

}

void K2QtTableModel::addColumnEF(const km::ColumnMetaData &column_meta, const QString &expr)
{
    if(getTableType() != TableType::TABLE){
        qDebug("Warning : Insertion of a column in a View.");
        return;
    }

    beginInsertColumns(QModelIndex(),columnCount(),columnCount());
    km::Table *table = dynamic_cast<km::Table*>(getAbsTable());
    QString error;
    SwitchLogHandlerHelper_ hdlr_locker([&error](const std::string &str){
        error = K_SQ_STR(str);
    });
    bool b = table->addColumnE(column_meta,expr.trimmed().replace('\n',' ').replace('\t',' ').toStdString());
    endInsertColumns();
    if(b)
    {
        m_stylers.push_back(k2qt::getCellStyler(nullptr,getUniqueName(),column_meta));
        setModifiedStatus(true);
        emit newColumnAdded(columnCount()-1, column_meta, 1, km::Variant(), expr);
    }
    else
    {
        beginRemoveColumns(QModelIndex(), columnCount(), columnCount());
        endRemoveColumns();
        CustomErrMsgBox::show(nullptr,tr("Add Column Error"),error);
    }
}

void K2QtTableModel::addColumnAI(const km::ColumnMetaData &column_meta)
{
    K_RETURN_IF_NOT(column_meta.data_type == km::DataType::INT32 || column_meta.data_type == km::DataType::INT64);
    if(getTableType() != TableType::TABLE){
        qDebug("Warning : Insertion of a column in a View.");
        return;
    }

    beginInsertColumns(QModelIndex(),columnCount(),columnCount());
    km::Table *table = dynamic_cast<km::Table*>(getAbsTable());
    QString error;
    SwitchLogHandlerHelper_ hdlr_locker([&error](const std::string &str){
        error = K_SQ_STR(str);
    });

    auto lambda1 = [](km::IndexType index) -> km::Variant { return km::KInt32(index);};
    auto lambda2 = [](km::IndexType index) -> km::Variant { return km::KInt64(index);};

    bool b = table->addColumnF(column_meta, (column_meta.data_type == km::DataType::INT32)? lambda1 : lambda2);
    endInsertColumns();
    if(b)
    {
        m_stylers.push_back(k2qt::getCellStyler(nullptr, getUniqueName(),column_meta));
        setModifiedStatus(true);
        emit newColumnAdded(columnCount()-1,column_meta, 2, km::Variant(), QString());
    }
    else
    {
        beginRemoveColumns(QModelIndex(), columnCount(), columnCount());
        endRemoveColumns();
        CustomErrMsgBox::show(nullptr,tr("Add Column Error"),error);
    }
}

void K2QtTableModel::transformColumnEF(km::IndexType column_index, QString expr)
{
    if(getTableType() != TableType::TABLE){
        qDebug("Warning : Insertion of a column in a View.");
        return;
    }
    if(column_index >= m_table->columnCount())
        return;

    beginResetModel();

    km::Table *table = dynamic_cast<km::Table*>(getAbsTable());
    QString error;
    SwitchLogHandlerHelper_ hdlr_locker([&error](const std::string &str){
        error = K_SQ_STR(str);
    });
    auto result = table->transformColumn(table->getColumnMetaData(column_index).column_name,
                                         expr.replace('\t',' ').replace('\n',' ').toStdString());
    endResetModel();
    if(!result)
        CustomErrMsgBox::show(nullptr,tr("Transform Error"),error);
    else
        setModifiedStatus(true);
}

void K2QtTableModel::transformColumnAI(km::IndexType column_index, int initial_value)
{
    if(getTableType() != TableType::TABLE){
        qDebug("Warning : Insertion of a column in a View.");
        return;
    }
    if(column_index >= m_table->columnCount() ||
            !(static_cast<bool>(m_table->getColumnMetaData(column_index).data_type & (km::DataType::INT32 | km::DataType::INT64))))
        return;

    beginResetModel();
    auto lambda1 = [](km::IndexType index, int initial_value) -> km::Variant { return km::KInt32(initial_value + index);};
    auto lambda2 = [](km::IndexType index, int initial_value) -> km::Variant { return km::KInt64(initial_value + index);};

    auto final_lambda = m_table->getColumnMetaData(column_index).data_type == km::DataType::INT32 ? lambda1 : lambda2;
    km::Table *table = dynamic_cast<km::Table*>(getAbsTable());
    for(km::IndexType i = 0, count = m_table->rowCount(); i < count; ++i)
        table->setData(i,column_index, final_lambda(i, initial_value));
    endResetModel();
    setModifiedStatus(true);
}

void K2QtTableModel::parentModelAboutToDestroy()
{
    writeSettings();
}

void K2QtTableModel::setDataFormat(km::IndexType column_index, unsigned style_index)
{
    auto mdata = getColumnMetaData(column_index);
    if(!mdata) return;

    k2qt::CellStyler &styler = m_stylers[column_index];
    styler.style_index = style_index;
    switch(mdata.value().data_type)
    {
    case km::DataType::INT32:
        styler.format = k2qt::getStandardInt32Formatter(style_index);
        break;
    case km::DataType::INT64:
        styler.format = k2qt::getStandardInt64Formatter(style_index);
        break;
    case km::DataType::FLOAT32:
        styler.format = k2qt::getStandardFloat32Formatter(style_index);
        break;
    case km::DataType::FLOAT64:
        styler.format = k2qt::getStandardFloat64Formatter(style_index);
        break;
    case km::DataType::STRING:
        styler.format = k2qt::getStandardStringFormatter(style_index);
        break;
    case km::DataType::BOOLEAN:
        styler.format = k2qt::getStandardBooleanFormatter(style_index);
        break;
    case km::DataType::DATE:
        styler.format = k2qt::getStandardDateFormatter(style_index);
        break;
    case km::DataType::DATE_TIME:
        styler.format = k2qt::getStandardDateTimeFormatter(style_index);
        break;
    }
}

void K2QtTableModel::setAlignment(km::IndexType column_index, Qt::AlignmentFlag h, Qt::AlignmentFlag v)
{
    if(column_index >= m_table->columnCount())
        return;
    auto &styler = m_stylers[column_index];
    styler.hrzt_align = h;
    styler.vrtc_align = v;
}

void K2QtTableModel::setStyler(const k2qt::CellStyler &styler, km::IndexType column_index)
{
    if(column_index < m_stylers.size())
        m_stylers[column_index] = styler;
}

void K2QtTableModel::setBackgroundColor(const QColor &color, km::IndexType column_index)
{
    if(column_index < m_stylers.size())
        m_stylers[column_index].bg_color = color;
}

void K2QtTableModel::setForegroundColor(const QColor &color, km::IndexType column_index)
{
    if(column_index < m_stylers.size())
        m_stylers[column_index].text_color = color;
}

QVariant K2QtTableModel::qtVariantFrom(const km::Variant &data) const
{
    km::DataType type = km::toDataType(data.index());
    switch(type){
    case km::DataType::INT32:
        return data.asInt32();
    case km::DataType::INT64:
        return data.asInt64();
    case km::DataType::FLOAT32:
        return QVariant(data.asFloat32());
    case km::DataType::FLOAT64:
        return data.asFloat64();
    case km::DataType::STRING:
        return QString::fromStdString(data.asString());
    case km::DataType::BOOLEAN:
        return data.asBoolean();
    case km::DataType::DATE:
    {
        km::KDate date = data.asDate();
        return QDate{date.year,date.month,date.day};
    }
    case km::DataType::DATE_TIME:
    {
        km::KDateTime dt = data.asDateTime();
        return QDateTime{
            QDate(dt.date.year,dt.date.month,dt.date.day),
                    QTime(dt.time.hour,dt.time.minute,dt.time.second)
        };
    }

    }
    return QVariant();
}

km::Variant K2QtTableModel::kmVariantFrom(const QVariant &data, km::DataType type) const
{
    using namespace km::tp;
    switch(type)
    {
    case km::DataType::INT32:
        return KInt32(data.toInt());
    case km::DataType::INT64:
        return KInt64(data.toLongLong());
    case km::DataType::FLOAT32:
        return KFloat32(data.toFloat());
    case km::DataType::FLOAT64:
        return KFloat64(data.toDouble());
    case km::DataType::STRING:
        return data.toString().toStdString();
    case km::DataType::BOOLEAN:
        return data.toBool();
    case km::DataType::DATE:
    {
        QDate date = data.toDate();
        return KDate{
            static_cast<uint16_t>(date.year()),
                    static_cast<uint8_t>(date.month()),
                    static_cast<uint8_t>(date.day())
        };
    }
    case km::DataType::DATE_TIME:
    {
        QDateTime dt = data.toDateTime();
        QDate date = dt.date();
        QTime time = dt.time();
        return KDateTime{{
                static_cast<uint16_t>(date.year()),
                        static_cast<uint8_t>(date.month()),
                        static_cast<uint8_t>(date.day())
            },
            {
                static_cast<uint8_t>(time.hour()),
                        static_cast<uint8_t>(time.minute()),
                        static_cast<uint8_t>(time.second())
            }
        };
    }
    }
    return km::Variant();
}

void K2QtTableModel::setModifiedStatus(bool modified)
{
    if(m_modified != modified)
    {
        m_modified = modified;
        emit modifiedStatusChanged(modified);
    }
}

void K2QtTableModel::writeSettings()
{
    K_RETURN_IF_NOT(getTableType() == TableType::TABLE);
    QSettings settings(getPath() % "/settings.ini", QSettings::IniFormat);
    auto children = findChildren<K2QtTableModel*>("K2QtTableModel");
    children.prepend(this);
    for(auto child : children)
    {
        for(km::IndexType i = 0, column_count = child->columnCount(); i < column_count; ++i)
        {
            k2qt::writeSettings(
                        child->m_stylers[i],
                        &settings,
                        child->getColumnMetaData(i).value(),
                        child->getUniqueName()
                        );
        }
    }
}

