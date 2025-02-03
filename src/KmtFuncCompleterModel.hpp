#ifndef KMTFUNCCOMPLETERMODEL_HPP
#define KMTFUNCCOMPLETERMODEL_HPP

#include <QAbstractListModel>

class KmtFuncCompleterModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit KmtFuncCompleterModel(QObject *parent = nullptr);
    void setColumnNames(QStringList column_names);
protected:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
private:
    static void initializeFunctionList();
private:
    QStringList m_column_names;
    static QStringList s_function_list;
};

#endif // KMTFUNCCOMPLETERMODEL_HPP
