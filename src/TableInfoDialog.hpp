#ifndef TABLEINFODIALOG_HPP
#define TABLEINFODIALOG_HPP

#include <QDialog>


class K2QtTableModel;


namespace Ui {
class TableInfoDialog;
}

/**
 * Dialog which is shown when we right click on opened table in project view
 * and click "Show Details" menu action.
 */
class TableInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TableInfoDialog(K2QtTableModel *model,QWidget *parent = nullptr);
    ~TableInfoDialog();

    static void showDetails(K2QtTableModel *model,QWidget *parent = nullptr);
private:
    Ui::TableInfoDialog *m_ui;
};

#endif // TABLEINFODIALOG_HPP
