#ifndef MANIPULATORHELPERFNCS__HPP
#define MANIPULATORHELPERFNCS__HPP

#include <QDate>
#include <QWidget>

#include <kmt/Core.hpp>


using ConvFunc_ = km::Variant(*)(QWidget*, QString*);

QWidget *createInputWidget(km::DataType data_type, const km::Variant &var, QWidget *parent);
QWidget *createInputWidget(km::DataType data_type, const QString &data, QWidget *parent);

km::Variant getInt32(QWidget *wdgt, QString *error = nullptr);

km::Variant getInt64(QWidget *wdgt, QString *error = nullptr);

km::Variant getFloat32(QWidget *wdgt, QString *error = nullptr);

km::Variant getFloat64(QWidget *wdgt, QString *error = nullptr);

km::Variant getString(QWidget *wdgt, QString *error = nullptr);

km::Variant getBoolean(QWidget *wdgt, QString *error = nullptr);

km::Variant getDate(QWidget *wdgt, QString *error = nullptr);

km::Variant getDateTime(QWidget *wdgt, QString *error = nullptr);

ConvFunc_ getConvFuncFor(km::DataType datatype);

QString createDefaultValueFor(km::DataType data_type);
km::Variant createDefaultVarValueFor(km::DataType data_type);

#endif // MANIPULATORHELPERFNCS__HPP
