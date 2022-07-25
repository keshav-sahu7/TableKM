#include "TablePreview.hpp"
#include <QFileInfo>
#include <fstream>

/**
 * Copied from KMTable@TableIO.cpp file.
 */
#define MAKE_R reinterpret_cast<char*>
static std::ifstream &readLString(std::ifstream &ifs, std::string &str)
{
    km::SizeType length = 0;
    ifs.read(MAKE_R(&length), sizeof(km::SizeType));
    auto cur_pos = ifs.tellg();
    ifs.seekg(0,std::ios::end);
    if(length <= static_cast<km::SizeType>((ifs.tellg() - cur_pos)))
    {
        ifs.seekg(cur_pos);
        str.resize(length);
        ifs.read(str.data(), length);
    }
    else
        ifs.setstate(std::ios::failbit);
    return ifs;
}


TablePreview::TablePreview(const QString &table_file)
{
    QFileInfo finfo(table_file);
    if( finfo.suffix() != "kmt" || !finfo.exists()){
        m_valid = false;
        return;
    }
    std::ifstream ifs(table_file.toStdString(), std::ios_base::binary | std::ios_base::in);
    m_valid = (readLString(ifs, m_table_name)
               && ifs.read(MAKE_R(&m_s_order), sizeof(m_s_order))
               && ifs.read(MAKE_R(&m_column_count), sizeof(km::SizeType))
               && ifs.read(MAKE_R(&m_row_count), sizeof(km::SizeType)));
    if(!m_valid)
        return;
    for (km::IndexType c = 0; c < m_column_count; ++c)
    {
        km::DataType column_type;
        std::string column_name;
        std::string display_name;
        m_valid = (ifs.read(MAKE_R(&column_type), sizeof(km::DataType))
                   && readLString(ifs, column_name)
                   && readLString(ifs, display_name));
        if(!m_valid)
            return;
        m_columns.push_back({column_name, display_name, column_type});
    }
    ifs.close();
}








