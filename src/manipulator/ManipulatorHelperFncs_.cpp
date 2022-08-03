#include "ManipulatorHelperFncs_.hpp"


#include <QComboBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QToolTip>

#include "../misc/ValidatorFactory.hpp"
#include "../misc/KDefines.h"
#include "../misc/MiscFunctions_.hpp"


QWidget *createInputWidget(km::DataType data_type, const km::Variant &var, QWidget *parent)
{
    if(data_type != km::toDataType(var.index()))
        return nullptr;
    auto str = toQString(var);
    return createInputWidget(data_type, str, parent);
}

QWidget *createInputWidget(km::DataType data_type, const QString &data, QWidget *parent)
{
    using dt = km::DataType;
    switch(data_type)
    {
    case dt::INT32:
    case dt::INT64:
    case dt::FLOAT32:
    case dt::FLOAT64:
    {
        QLineEdit *line_edit = new QLineEdit(parent);
        line_edit->setValidator(ValidatorFactory::instance().validator(data_type));
        line_edit->setText(data);
        return line_edit;
    }
    case dt::STRING:
    {
        return new QLineEdit(data,parent);
    }
    case dt::BOOLEAN:
    {
        auto cb = new QComboBox(parent);
        cb->addItems({"True","False"});
        cb->setCurrentIndex(data.trimmed().toLower() == "true" ? 0 : 1);
        return cb;
    }
    case dt::DATE:
    {
        QDateEdit *date_edit = new QDateEdit(parent);
        date_edit->setDate(QDate::fromString(data,"dd/MM/yyyy"));
        date_edit->setCalendarPopup(true);
        return date_edit;
    }
    case dt::DATE_TIME:
    {
        QDateTimeEdit *date_time_edit = new QDateTimeEdit(parent);
        date_time_edit->setDateTime(QDateTime::fromString(data,"dd/MM/yyyy hh:mm:ss"));
        date_time_edit->setCalendarPopup(true);
        return date_time_edit;
    }
    }
    return nullptr;
}

km::Variant getInt32(QWidget *wdgt, QString *error)
{
    QLineEdit *line_edit = dynamic_cast<QLineEdit*>(wdgt);
    km::KInt32 value;
    bool ok;
    if constexpr(sizeof(int) == sizeof(km::KInt32))
        value = line_edit->text().toLong(&ok);
    else
        value = line_edit->text().toInt(&ok);
    if(!ok && error)
        *error = "Number Out Of Range";
    return value;
}

km::Variant getInt64(QWidget *wdgt, QString *error)
{
    QLineEdit *line_edit = dynamic_cast<QLineEdit*>(wdgt);
    bool ok;
    km::KInt64 value = line_edit->text().toLongLong(&ok);       //assuming long long to be 8 bytes
    if(!ok && error)
        *error = "Number is out of range";
    return value;
}

km::Variant getFloat32(QWidget *wdgt, QString *error)
{
    QLineEdit *line_edit = dynamic_cast<QLineEdit*>(wdgt);
    bool ok;
    km::KFloat32 value = line_edit->text().toFloat(&ok);        //assuming float to be 4 byte or above
    if(!ok && error)
        *error = "Number is out of range";
    return value;
}

km::Variant getFloat64(QWidget *wdgt, QString *error)
{
    QLineEdit *line_edit = dynamic_cast<QLineEdit*>(wdgt);
    bool ok;
    km::KFloat64 value = line_edit->text().toDouble(&ok);        //assuming double to be 4 byte or above
    if(!ok && error)
        *error = "Number is out of range";
    return value;
}

km::Variant getString(QWidget *wdgt, [[maybe_unused]] QString *error)
{
    QLineEdit *line_edit = dynamic_cast<QLineEdit*>(wdgt);
    return line_edit->text().toStdString();
}

km::Variant getBoolean(QWidget *wdgt, [[maybe_unused]] QString *error)
{
    QComboBox *check_box = dynamic_cast<QComboBox*>(wdgt);
    return km::KBoolean(check_box->currentIndex() == 0);
}

km::Variant getDate(QWidget *wdgt, QString *error)
{
    QDateEdit *date_edit = dynamic_cast<QDateEdit*>(wdgt);
    QDate date = date_edit->date();
    bool ok = date.isValid();
    if(!ok && error)
        *error = "Date is not valid";
    return to_kdate(date.year(),date.month(),date.day());
}

km::Variant getDateTime(QWidget *wdgt, QString *error)
{
    QDateTimeEdit *date_time_edit = dynamic_cast<QDateTimeEdit*>(wdgt);
    QDate date = date_time_edit->date();
    QTime time = date_time_edit->time();
    bool ok = date_time_edit->dateTime().isValid();
    if(!ok && error)
        *error = "DateTime is not valid.";
    return to_kdateTime(date.year(),date.month(),date.day(),time.hour(),time.minute(),time.second());
}

ConvFunc_ getConvFuncFor(km::DataType datatype)
{
    ConvFunc_ array[] = {
        getInt32,
        getInt64,
        getFloat32,
        getFloat64,
        getString,
        getBoolean,
        getDate,
        getDateTime,
        nullptr
    };
    return array[km::indexForDataType(datatype,8)];
}

QString createDefaultValueFor(km::DataType data_type)
{
    if(static_cast<bool>(data_type & (km::DataType::INT32 | km::DataType::INT64 | km::DataType::FLOAT32 | km::DataType::FLOAT64)))
        return "0";
    else if(data_type == km::DataType::BOOLEAN)
        return "True";
    else if(data_type == km::DataType::DATE)
        return QDate::currentDate().toString("dd/MM/yyyy");
    else if(data_type == km::DataType::DATE_TIME)
        return QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
    return QString();
}

km::Variant createDefaultVarValueFor(km::DataType data_type)
{
    auto dt = QDateTime::currentDateTime();
    auto d = dt.date();
    auto t = dt.time();

    //make it static to make this function faster.
    static km::Variant default_values[] = {
        km::KInt32(0),
        km::KInt64(0),
        km::KFloat32(0),
        km::KFloat64(0),
        km::KString(),
        km::KBoolean(false),
        km::KDate({2022,1,1}),
        km::KDateTime({{2022,1,1},{0,0,0}})
    };

    //update it as it time of date_time would be changed every seconds.
    default_values[6] = to_kdate(d.year(),d.month(),d.day());
    default_values[7] = to_kdateTime(d.year(),d.month(),d.day(),t.hour(),t.minute(),t.second());
    return default_values[km::indexForDataType(data_type,0)];
}
