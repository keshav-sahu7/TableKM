#ifndef K2QTTABLEMODEL_H
#define K2QTTABLEMODEL_H

#include <functional>

#include <QAbstractTableModel>
#include <QStandardItem>

#include <kmt/AbstractTable.hpp>
#include <kmt/Table.hpp>
#include <kmt/BasicView.hpp>

#include "datafmts/CommonDataFormats.hpp"
#include "datafmts/CellStyler.hpp"
#include "misc/KDefines.h"

class QStandardItem;

enum class TableType
{
    TABLE,
    BASIC_VIEW
};

class TableView;

/**
 * This class is used for showing data from km::Table and km::BasicView in a QTableView.
 * It also adds other features that km classes doesn't have.
 */
class K2QtTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    // constructor for table and view
    explicit K2QtTableModel(km::Table *table, const QString &path, QObject *parent);
    explicit K2QtTableModel(km::BasicView *table, const QString &path, K2QtTableModel *parent);

    // destructor
    ~K2QtTableModel();

    // overriding QAbstractTableModel functions.
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    // get display names and column data type of all columns.
    std::vector<std::pair<std::string, km::DataType>> getColumnInfo() const;
    // get meta data of a single column.
    std::optional<km::ColumnMetaData> getColumnMetaData(int column_index) const;
    // get currently set styler on column at @c index
    const k2qt::CellStyler &getStyler(int column_index) const;
    // path where data will be saved
    const QString &getPath() const;
    // get table name
    QString getName() const;
    // get the underlying table
    km::AbstractTable *getAbsTable();
    const km::AbstractTable *getAbsTable() const;

    // get whether the it is table (mutable) or just a view (read only)
    TableType getTableType() const;
    // get the tableview it is opened in [Note : it will work with only one table widget].
    TableView *getViewWidget() const;
    // get item represeting in the project view
    QStandardItem *getProjectedItem() const;
    // get if table is opened in a view while the model still exists in the memory.
    bool isOpened() const;
    // get if TABLE is modified
    bool isModified() const;
    // get if changes of TABLE is reflected on hard drive
    bool isSaved() const;
    // save table, if forced=true, it will save it irrespective of table is modified or not.
    void saveTable(bool forced = false);
    // returns a string represents a parent-child-grandchild name
    QString getUniqueName() const;

    // set data format
    void setDataFormat(km::IndexType column_index, unsigned style_index);
    // set alignment
    void setAlignment(km::IndexType column_index, Qt::AlignmentFlag h, Qt::AlignmentFlag v);
    // set styler
    void setStyler(const k2qt::CellStyler &styler, km::IndexType column_index);
    // set cell color
    void setBackgroundColor(const QColor &color, km::IndexType column_index);
    // set text color
    void setForegroundColor(const QColor &color, km::IndexType column_index);
public slots:

    // add row
    void addRow(const std::vector<km::Variant> &elements);
    // drop row
    void dropRow(km::IndexType index);

    // add column fill value
    void addColumnFV(const km::ColumnMetaData &column_meta, const km::Variant &fill_with);
    // add column execute formulae
    void addColumnEF(const km::ColumnMetaData &column_meta, const QString &expr);
    // add column auto increament
    void addColumnAI(const km::ColumnMetaData &column_meta);

    // transform with formula
    void transformColumnEF(km::IndexType column_index, QString expr);
    // transform with auto increament
    void transformColumnAI(km::IndexType column_index, int initial_value);

    // save settings of views before parent model dies
    void parentModelAboutToDestroy();
signals:
    // set it modified.
    void modifiedStatusChanged(bool modified);
    //fill_type 0:value, 1:expr, 2:auto_inc
    void newColumnAdded(km::IndexType index, const km::ColumnMetaData &column_meta, int fill_type, const km::Variant &value, const QString &expr);
private:
    // actual private constructor
    explicit K2QtTableModel(km::AbstractTable *table, const QString &path, TableType table_type, QObject *parent);

    //km::Variant to QVariant and vice-versa
    QVariant qtVariantFrom(const km::Variant &data) const;
    km::Variant kmVariantFrom(const QVariant &data, km::DataType type) const;

    // table view
    void setViewWidget(TableView *widget);
    // project view item
    void setProjectItem(QStandardItem *item);
    // set table is modified(true)/saved(false).
    void setModifiedStatus(bool modified);
    // save format settings
    void writeSettings();
private:
    std::unique_ptr<km::AbstractTable> m_table;     // the actual data
    std::vector<k2qt::CellStyler> m_stylers;        // column formatters
    QString m_path;                     // root path where table data will be saved (usually any_path/table_name)
    TableType m_ttype;                  // type of the table (table|view)
    TableView *m_guiview;               // the table view (only one table can have this model at a time)
    QStandardItem *m_prjitem;           // item in tree view (project view)
    bool m_modified;                    // modify status

private:
    friend class TableView;
    friend class ProjectView;
};


inline const k2qt::CellStyler &K2QtTableModel::getStyler(int column_index) const
{
    return m_stylers[column_index];
}

inline const QString &K2QtTableModel::getPath() const
{
    return m_path;
}

inline QString K2QtTableModel::getName() const
{
    return K_SQ_STR(m_table->getName());
}

inline km::AbstractTable *K2QtTableModel::getAbsTable()
{
    return m_table.get();
}

inline const km::AbstractTable *K2QtTableModel::getAbsTable() const
{
    return m_table.get();
}

inline TableType K2QtTableModel::getTableType() const
{
    return m_ttype;
}

inline TableView *K2QtTableModel::getViewWidget() const
{
    return m_guiview;
}

inline QStandardItem *K2QtTableModel::getProjectedItem() const
{
    return m_prjitem;
}

inline bool K2QtTableModel::isOpened() const
{
    return static_cast<bool>(getViewWidget());
}

inline bool K2QtTableModel::isModified() const
{
    return m_modified;
}

inline bool K2QtTableModel::isSaved() const
{
    return !m_modified;
}

inline void K2QtTableModel::setViewWidget(TableView *widget)
{
    m_guiview = widget;
}

inline void K2QtTableModel::setProjectItem(QStandardItem *item)
{
    m_prjitem = item;
}

#endif // K2QTTABLEMODEL_H
