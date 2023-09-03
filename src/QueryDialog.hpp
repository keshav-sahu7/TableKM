#ifndef QUERYDIALOG_HPP
#define QUERYDIALOG_HPP

#include "KmtFuncPopupEditor.hpp"

#include <QDialog>

namespace Ui {
class QueryDialog;
}

class K2QtTableModel;
class TableView;

/**
 * SQL SELECT *like* query runner dialog.
 */
class QueryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QueryDialog(QList<K2QtTableModel*> models, QWidget *parent = nullptr);
    ~QueryDialog();
private slots:
    /// update the sort with column combo list.
    void updateNameListCB(int index);

    /// export file as csv
    void exportAsCSV();

    /// export file as text file
    void exportAsText();

    /// print table to pdf/printer
    void printTable();

    /// add query as view
    void addAsView();

    /// run the query
    void runQuery();

signals:
    /// passes the current view as view of the parent table to be added as view.
    void addView(K2QtTableModel *view, K2QtTableModel *parent);

private:
    /// validates fields before running the query.
    bool preValidate();
private:
    Ui::QueryDialog *m_ui;
    TableView *m_table_view;
    K2QtTableModel *m_current_parent;
    KmtFuncPopupEditor *m_popup_editor;
};

#endif // QUERYDIALOG_HPP
