#ifndef DELEGATES_HPP
#define DELEGATES_HPP

#include "NumericalDelegate.hpp"
#include "DateDelegate.hpp"
#include "DateTimeDelegate.hpp"


inline NumericalDelegate *getNumDlgt(const QString &pattern, QWidget *parent)
{
    NumericalDelegate *dlgt = new NumericalDelegate(nullptr,parent);
    dlgt->setValidator(pattern);
    return dlgt;
}

inline NumericalDelegate *getInt32Delegate(QWidget *parent)
{
    return getNumDlgt(QStringLiteral("-?\\d{1,10}"),parent);
}

inline NumericalDelegate *getInt64Delegate(QWidget *parent)
{
    return getNumDlgt(QStringLiteral("-?\\d{1,19}"),parent);
}

inline NumericalDelegate *getFloat32Delegate(QWidget *parent)
{
    return getNumDlgt(QStringLiteral("-?\\d+(\\.\\d*([eE][\\+-]\\d{1,2})?)?"),parent);
}

inline NumericalDelegate *getFloat64Delegate(QWidget *parent)
{
    return getNumDlgt(QStringLiteral("-?\\d+(\\.\\d*([eE][\\+-]\\d{1,2})?)?"),parent);
}

#endif // DELEGATES_HPP
