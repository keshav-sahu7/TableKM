#ifndef OPENTABLEDIALOG_HPP
#define OPENTABLEDIALOG_HPP

#include <QDialog>
#include <QStandardItemModel>

#include <kmt/Table.hpp>

namespace Ui {
class OpenTableDialog;
}

class OpenTableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenTableDialog(QWidget *parent = nullptr);
    QString getPath() const;
    QString tableName() const;
    QList<km::AbstractView*> &getViews();
    ~OpenTableDialog();
    km::Table *getTable() const;
private slots:
    void showPreview(const QString &file);
    void browsePath();
    void onOpen();
private:
    Ui::OpenTableDialog *m_ui;
    QStandardItemModel *m_model;
    km::Table *m_table;
    QList<km::AbstractView*> m_views;
};

inline QList<km::AbstractView *> &OpenTableDialog::getViews()
{
    return m_views;
}

inline km::Table *OpenTableDialog::getTable() const
{
    return m_table;
}


#endif // OPENTABLEDIALOG_HPP
