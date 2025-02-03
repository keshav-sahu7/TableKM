#include "KmtFuncCompleterModel.hpp"
#include "ThemeHandler2.hpp"

#include <QFile>
#include <QIcon>

QStringList KmtFuncCompleterModel::s_function_list = QStringList();

KmtFuncCompleterModel::KmtFuncCompleterModel(QObject *parent)
    : QAbstractListModel{parent}
{
    if(s_function_list.isEmpty())
        initializeFunctionList();
}

void KmtFuncCompleterModel::setColumnNames(QStringList column_names)
{
    m_column_names = column_names;
}

int KmtFuncCompleterModel::rowCount(const QModelIndex &parent) const
{
    return m_column_names.count() + s_function_list.count();
}

QVariant KmtFuncCompleterModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() && index.row() >= rowCount(index))
        return QVariant{};

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.row() < m_column_names.count())
            return m_column_names[index.row()];
        else
            return s_function_list[index.row() - m_column_names.count()];
    }
    else if(role == Qt::DecorationRole)
    {
        if (index.row() < m_column_names.count())
            return icons::getIcon("c-solid");
        else
            return icons::getIcon("florin-sign-solid");
    }

    return QVariant{};
}

void KmtFuncCompleterModel::initializeFunctionList()
{
    QFile file(":/function-list.txt");
    if (!file.open(QFile::ReadOnly))
        return;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            s_function_list << QString::fromUtf8(line.trimmed());
    }

    s_function_list.removeIf([](const QString& function_name){
        return function_name.isEmpty();
    });
}

