#ifndef CREATETABLEDIALOG_HPP
#define CREATETABLEDIALOG_HPP

#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>

#include <kmt/Table.hpp>

/**
 * The dialog window that opens a km::Table and all dependent
 * views and returns to the main window.
 */
class CreateTableDialog : public QDialog
{
    Q_DISABLE_COPY_MOVE(CreateTableDialog)
public:
    CreateTableDialog(QWidget *parent = nullptr);
    static std::tuple<km::Table *, QString> getTable(QWidget *parent);
    km::Table *getTable();
    QString getPath() const;
    //std::vector<QString> getDefaultValues() const;
    ~CreateTableDialog() = default;
private slots:
    void insertColumn();
    void removeColumn();
    void moveUpColumn();
    void moveDownColumn();
    void createTable();
    void browsePath();
    void datatypeChanged(int index);
private:
    //QValidator *createDefaultValueWidget(km::DataType data_type);
private:
    QLineEdit *m_tname;
    QLineEdit *m_tspath;
    QComboBox *m_sorting_order;
    QTableWidget *m_column_table;
    km::Table *m_table;
    //std::vector<QString> m_vec;
};

inline km::Table *CreateTableDialog::getTable()
{
    return m_table;
}

//inline std::vector<QString> CreateTableDialog::getDefaultValues() const
//{
//    return m_vec;
//}

#endif // CREATETABLEDIALOG_HPP
