
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QMap>
#include "DockWidget.hpp"
#include <kmt/Table.hpp>

class TableView;


class ProjectView;
class QStandardItem;

class K2QtTableModel;
namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void showAboutQtDialog();
    void showAboutUsDialog();
    void onQuery();
    void openExistingTable();
    void createNewTable();
    void saveOpenedTable(K2QtTableModel *model);
    void closeTab(int index);
    void addView(K2QtTableModel *model, K2QtTableModel *parent);
    void openDocumentationPage();
    void openWelcomePage();
    void addThirdPartyWidget(QWidget *tab, const QString &label, int index = -1, const QIcon &icon = QIcon());
    /**
     * @brief slot for K2QtTableModel modified signal.
     * 
     * This slot shows the opened table tab as modified/saved depending on the @b modified . If @b modified is
     * true, the tab text of the opened table will be bold and a * will be prepended.
     * 
     * @note This function should be called by a K2QtTableModel opened in the tab widget. Else it will be ignored.
     */
    void modelModifiedStatusChanged(bool modified);

    //requires typeof(sender()) = "QAction*" and typeof(sender().data()) = "K2QtTableModel*"
    void createNewView();
    void deleteView();
    void openInEditor();
    void closeEditor();
    void saveOpenedTable();
private:
    void popupContextMenu(const QPoint &point);
private:
    void setMenu();
    void installDockWidgets();
    K2QtTableModel *addTable(km::Table *table,
                             const QString &save_path/*,
                             const std::vector<QString> &default_values*/);
private:
    Ui::MainWindow *m_ui;
    //QMap<QString,DockWidget*> m_dock_widgets;
    ProjectView *m_project_view;
};
#endif // MAINWINDOW_H
