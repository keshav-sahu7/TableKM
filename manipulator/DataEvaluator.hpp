#ifndef DATAEVALUATOR_HPP
#define DATAEVALUATOR_HPP

#include <memory>
#include <vector>

#include <QObject>

#include <kmt/Table.hpp>
#include <kmt/Parser2.hpp>

class K2QtTableModel;
class AddRowWidget;

class FunctionLocker_{
public:
    void lock() { m_locked = true;}
    void unlock() { m_locked = false;}
    bool isLocked() { return m_locked;}
private:
    bool m_locked = true;
};

class DataEvaluator : public QObject
{
public:
    struct EvalData_
    {
        QWidget *field;
        ////expr and compiled_token will be used in case of EXPR////
        QString expr;
        km::parse::TokenContainer compiled_token;
        ////value will be used in case of VALUE and AUTO_INC////
        km::Variant value;
        enum FillType_{
            VALUE = 0,
            EXPR = 1,
            AUTO_INC = 2
        } fill_type;
        km::Variant(*getter)(QWidget*, QString*);
    };

public:
    DataEvaluator(AddRowWidget *add_row_widget);
    const QList<EvalData_> &getEvalData();
    ~DataEvaluator();
public slots:
    void newColumnAdded(const km::ColumnMetaData &column_meta, const EvalData_ &column_detail);
    void valueChanged(km::IndexType column_index, const km::Variant &value);
    void autoIncreamentNeeded();
private:
    void initTable(const std::vector<km::ColumnMetaData> &column_meta_list);
    void setData(const km::Variant &data, QWidget *widget);
    void setEvalData(const QList<EvalData_> &eval_data);
    const km::ColumnMetaData &getColumnMetaData(km::IndexType column_index);
    const km::Table *getTable() const;
private:
    km::Table *m_table;
    QList<EvalData_> m_eval_data_list;
    FunctionLocker_ m_locker;
    QString m_eval_file;
    friend class AutoValueGeneratorSettingsWindow;
};

inline const km::Table *DataEvaluator::getTable() const
{
    return m_table;
}


#endif // DATAEVALUATOR_HPP
