#ifndef TABLEPREVIEW_H
#define TABLEPREVIEW_H

#include <kmt/Column.hpp>
#include <kmt/AbstractTable.hpp>

#include <QString>

class TablePreview
{
public:
    TablePreview(const QString &table_file);
    bool isValid() const;
    km::IndexType getRowCount() const;
    km::IndexType getColumnCount() const;
    const std::vector<km::ColumnMetaData> &getColumns() const;
    const std::string &getTableName() const;
    km::SortingOrder getSortingOrder() const;

private:
    bool m_valid;
    std::string m_table_name;
    std::vector<km::ColumnMetaData> m_columns;
    km::IndexType m_row_count;
    km::IndexType m_column_count;
    km::SortingOrder m_s_order;
};

inline bool TablePreview::isValid() const
{
    return m_valid;
}

inline km::IndexType TablePreview::getRowCount() const
{
    return m_row_count;
}

inline km::IndexType TablePreview::getColumnCount() const
{
    return m_column_count;
}

inline const std::vector<km::ColumnMetaData> &TablePreview::getColumns() const
{
    return m_columns;
}

inline const std::string &TablePreview::getTableName() const
{
    return m_table_name;
}

inline km::SortingOrder TablePreview::getSortingOrder() const
{
    return m_s_order;
}


#endif // TABLEPREVIEW_H
