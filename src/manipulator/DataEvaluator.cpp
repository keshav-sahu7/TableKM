#include "AddRowWidget.hpp"

#include <QComboBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QDateTimeEdit>

#include "DataEvaluator.hpp"
#include "../K2QtTableModel.hpp"
#include "ManipulatorHelperFncs_.hpp"
#include "../misc/MiscFunctions_.hpp"



DataEvaluator::DataEvaluator(AddRowWidget *add_row_widget)
    : QObject(add_row_widget),
      m_table(nullptr)
{
    assert(add_row_widget);

    K2QtTableModel *model = add_row_widget->getModel();
    m_eval_data_list.reserve(model->columnCount());

    std::vector<km::ColumnMetaData> columns;
    columns.reserve(model->columnCount());

    m_eval_file = model->getPath() + "/data_eval.ini";
    QSettings settings(m_eval_file,QSettings::IniFormat);
    for(int i = 0, count = model->columnCount(); i < count; ++i)
    {
        km::ColumnMetaData meta = model->getColumnMetaData(i).value();
        EvalData_ data;
        settings.beginGroup(K_SQ_STR(meta.column_name));

        data.field = add_row_widget->getFieldWidget(i).second;
        data.getter = add_row_widget->getFieldWidget(i).first;

        data.fill_type = static_cast<EvalData_::FillType_>(settings.value("fill_type", 0).toInt());
        QVariant value = settings.value("value");
        if(value.isValid())
        {
            data.value = toKVariant(value.toString(), meta.data_type);
        }
        else
        {
            data.value = createDefaultVarValueFor(meta.data_type);
        }

        if(data.fill_type == EvalData_::EXPR)
        {
            QString expr = settings.value("expr").toString();
            if(expr.isEmpty())
                expr = createDefaultValueFor(meta.data_type);
            data.expr = expr;
        }

        columns.push_back(meta);
        m_eval_data_list.append(data);
        settings.endGroup();
    }

    initTable(columns);
}

const QList<DataEvaluator::EvalData_> &DataEvaluator::getEvalData()
{
    for(km::IndexType i = 0, count = m_table->columnCount() - 1; i < count; ++i)
    {
        if(m_eval_data_list[i].fill_type == EvalData_::VALUE)
            m_eval_data_list[i].value = m_table->getData(0,i+1).value();
    }
    return m_eval_data_list;
}

const km::ColumnMetaData &DataEvaluator::getColumnMetaData(km::IndexType column_index)
{
    assert((column_index + 1) < m_table->columnCount());
    return m_table->getColumnMetaData(column_index + 1);
}

void DataEvaluator::setEvalData(const QList<EvalData_> &eval_data)
{
    m_eval_data_list = eval_data;

    m_locker.lock();
    for(km::IndexType i = 0, count = m_table->columnCount()-1; i < count; ++i)
    {
        auto &eval = m_eval_data_list[i];
        if(eval.fill_type == EvalData_::VALUE)
        {
            m_table->setData(0,i+1,eval.value);
            setData(eval.value, eval.field);
        }
    }
    m_locker.unlock();

    valueChanged(km::INVALID_INDEX, km::Variant());
}


DataEvaluator::~DataEvaluator()
{
    QSettings settings(m_eval_file,QSettings::IniFormat);
    for(km::IndexType i = 0, count = m_table->columnCount()-1; i < count; ++i)
    {
        const km::ColumnMetaData &column_meta = m_table->getColumnMetaData(i+1);
        const EvalData_ &data = m_eval_data_list[i];
        settings.beginGroup(K_SQ_STR(column_meta.column_name));

        settings.setValue("fill_type", static_cast<int>(data.fill_type));
        settings.setValue("value", toQString(data.value));
        if(data.fill_type == EvalData_::EXPR)
            settings.setValue("expr", data.expr);

        settings.endGroup();
    }

    delete m_table;
}

void DataEvaluator::newColumnAdded(const km::ColumnMetaData &column_meta, const EvalData_ &column_detail)
{
    m_eval_data_list.push_back(column_detail);
    if(m_table == nullptr)
    {
        initTable({});
        return;
    }

    //insert column
    m_table->addColumn(column_meta, createDefaultVarValueFor(column_meta.data_type));

    //reference to last eval_data (current eval_data)
    auto &eval_data_ref = m_eval_data_list.last();

    // column index
    const km::IndexType column_index = m_table->columnCount() - 1;

    // validator
    bool ok = true;
    if(column_detail.fill_type == EvalData_::VALUE)
    {
        ok = m_table->setData(0, column_index, eval_data_ref.value);
    }
    else if(column_detail.fill_type == EvalData_::EXPR)
    {
        ok = km::parse::getCheckedToken(eval_data_ref.expr.toStdString(), eval_data_ref.compiled_token, m_table, column_meta.data_type);
        if(ok)  // double check
            ok = m_table->setData(0, column_index, km::parse::evaluateFormula(eval_data_ref.compiled_token, m_table, 0));
    }
    //else AUTO_INC //do nothing

    if(!ok)         // everything fails ? set default to VALUE and set default value and clear formula
    {
        eval_data_ref.fill_type = EvalData_::VALUE;
        eval_data_ref.value = m_table->getDataWC(0,column_index);
        eval_data_ref.expr.clear();
        eval_data_ref.compiled_token.clear();
    }

    m_locker.lock();
    setData(m_table->getDataWC(0, column_index), m_eval_data_list.last().field);
    m_locker.unlock();
}

void DataEvaluator::valueChanged(km::IndexType column_index, const km::Variant &value)
{
    K_RETURN_IF_NOT(m_table);
    if(m_locker.isLocked())
    {
        return;
    }
    m_locker.lock();

    if(column_index != km::INVALID_INDEX)
    {
        m_table->setData(0,column_index+1, value);
    }

    km::IndexType i = (column_index == km::INVALID_INDEX) ? 0 : column_index+1;
    km::SizeType count = m_table->columnCount()-1;

    for(;i < count; ++i)
    {
        if(m_eval_data_list[i].fill_type == EvalData_::EXPR)
        {
            km::parse::evaluateFormula(m_eval_data_list[i].compiled_token, m_table, i+1, 0, 0);
            auto data = m_table->getDataWC(0,i+1);
            setData(data,m_eval_data_list[i].field);
        }
    }
    m_locker.unlock();
}

void DataEvaluator::autoIncreamentNeeded()
{
    km::IndexType first_ai_index = km::INVALID_INDEX;
    km::Variant first_changed_data;
    m_locker.lock();
    for(km::IndexType i = 0, count = m_table->columnCount()-1;i < count; ++i)
    {
        if(m_eval_data_list[i].fill_type == EvalData_::AUTO_INC)
        {
            QString error;
            km::Variant old_data = m_eval_data_list[i].getter(m_eval_data_list[i].field, &error);
            if(error.isEmpty())
            {
                km::Variant new_data = (old_data.index() == 0) ?
                            km::Variant(km::KInt32(old_data.asInt32() + 1)) :
                            km::Variant(km::KInt64(old_data.asInt64() + 1));
                m_table->setData(0,i+1, new_data);
                first_ai_index = i;
                setData(new_data, m_eval_data_list[i].field);
                first_changed_data = new_data;
            }
        }
    }
    m_locker.unlock();
    if(first_ai_index != km::INVALID_INDEX)
        valueChanged(first_ai_index, first_changed_data);
}

//void DataEvaluator::autoIncreamentNeeded()
//{
//    K_RETURN_IF_NOT(m_table);
//    m_locker.lock();
//    for(km::IndexType i = 0, count = m_table->columnCount()-1;i < count; ++i)
//    {
//        if(m_eval_data_list[i].fill_type == EvalData_::AUTO_INC)
//        {
//            QString error;
//            km::Variant old_data = m_eval_data_list[i].getter(m_eval_data_list[i].field, &error);
//            if(error.isEmpty())
//            {
//                km::Variant new_data = (old_data.index() == 0) ?
//                            km::KInt32(old_data.asInt32() + 1) :
//                            km::KInt64(old_data.asInt64() + 1);
//                m_table->setData(0,i+1, new_data);
//                setData(new_data, m_eval_data_list[i].field);
//            }
//        }
//        else if(m_eval_data_list[i].fill_type == EvalData_::EXPR)
//        {
//            km::parse::evaluateExpression(m_eval_data_list[i].compiled_token, m_table, i+1, 0, 0);
//            auto new_data = m_table->getDataWC(0,i+1);
//            setData(new_data,m_eval_data_list[i].field);
//        }
//    }
//    m_locker.unlock();
//}


void DataEvaluator::initTable(const std::vector<km::ColumnMetaData> &column_meta_list)
{
    try{
        std::vector<km::ColumnMetaData> column_meta_list_copy = {
            //ignore this column, it will be used as key column which can not be changed.
            km::ColumnMetaData("__data_evaluator_a5p4g6h8_key", km::DataType::INT32)
        };
        std::copy(column_meta_list.begin(),column_meta_list.end(), std::back_inserter(column_meta_list_copy));
        m_table = new km::Table("__data_evaluator_", column_meta_list_copy);
    }
    catch(...)
    {
        return;
    }

    std::vector<km::Variant> row = {km::KInt32(0)};   //value for __data_evaluator_a5p4g6h8_key column
    row.reserve(m_table->columnCount());

    m_locker.lock();
    // here m_table looks like [A0 A1 A2 A3 ... An]
    // and m_eval_data looks like [D0 D1 D2 D3 ... Dn-1]
    // Because A0 represents internal key which is already set to 0 in above row declaration
    // So now D0 -> A1, D1 -> A2, D2 -> A3 ... Dn-1 -> An. Hence loop runs for i = 0 to n-1.
    for(km::IndexType i = 0, count = m_table->columnCount()-1; i < count; ++i)
    {
        if(m_eval_data_list[i].fill_type == EvalData_::EXPR)
        {
            if(!km::parse::getCheckedToken(m_eval_data_list[i].expr.toStdString(), m_eval_data_list[i].compiled_token, m_table, column_meta_list[i].data_type))
            {
                m_eval_data_list[i].expr.clear();
                m_eval_data_list[i].compiled_token.clear();
                m_eval_data_list[i].fill_type = EvalData_::VALUE;
            }
        }
        else
            setData(m_eval_data_list[i].value, m_eval_data_list[i].field);
        row.push_back(m_eval_data_list[i].value);
    }
    m_locker.unlock();
    m_table->insertRow(row);
    valueChanged(km::INVALID_INDEX, km::Variant());
}

void DataEvaluator::setData(const km::Variant &data, QWidget *widget)
{
    switch(data.index())
    {
    case 0:
        if(auto w = dynamic_cast<QLineEdit*>(widget))
        {
            w->setText(QString::number(data.asInt32()));
        }
        break;
    case 1:
        if(auto w = dynamic_cast<QLineEdit*>(widget))
        {
            w->setText(QString::number(data.asInt64()));
        }
        break;
    case 2:
        if(auto w = dynamic_cast<QLineEdit*>(widget))
        {
            w->setText(QString::number(data.asFloat32()));
        }
        break;
    case 3:
        if(auto w = dynamic_cast<QLineEdit*>(widget))
        {
            w->setText(QString::number(data.asFloat64()));
        }
        break;
    case 4:
        if(auto w = dynamic_cast<QLineEdit*>(widget))
        {
            w->setText(K_SQ_STR(data.asString()));
        }
        break;
    case 5:
        if(auto w = dynamic_cast<QComboBox*>(widget))
        {
            w->setCurrentIndex(data.asBoolean() ? 0 : 1);
        }
        break;
    case 6:
        if(auto w = dynamic_cast<QDateEdit*>(widget))
        {
            auto d = data.asDate();
            w->setDate(QDate(d.year, d.month, d.day));
        }
        break;
    case 7:
        if(auto w = dynamic_cast<QDateTimeEdit*>(widget))
        {
            auto d = data.asDateTime();
            w->setDateTime({QDate(d.date.year, d.date.month, d.date.day), QTime(d.time.hour,d.time.minute,d.time.second)});
        }
        break;
    }
}
