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
    void sourceTableChanged(int index);
    void exportAsCSV();
    void exportAsText();
    void printTable();
    void addAsView();
    void runQuery();
signals:
    void addView(K2QtTableModel *view, K2QtTableModel *parent);
private:
    bool preValidate();
private:
    Ui::QueryDialog *m_ui;
    TableView *m_table_view;
    K2QtTableModel *m_current_parent;
    KmtFuncPopupEditor *m_popup_editor;
};

#endif // QUERYDIALOG_HPP
