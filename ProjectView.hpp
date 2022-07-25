#ifndef PROJECTVIEW_HPP
#define PROJECTVIEW_HPP

#include <QTreeWidget>
#include <QObject>
#include <QStandardItemModel>
#include <kmt/AbstractTable.hpp>

class K2QtTableModel;
class MainWindow;

class ProjectView : public QTreeView
{
public:
    ProjectView(MainWindow *main_window, QWidget *parent = nullptr);
    QStandardItem* addTable(K2QtTableModel *table);
    QStandardItem* addView(K2QtTableModel *view, K2QtTableModel *parent);
    void remove(K2QtTableModel *table);
    K2QtTableModel *modelAt(const QPoint &point);
    QStandardItem *findItem(km::AbstractTable *table, QStandardItem *root = nullptr);
    QStandardItem *findTableItem(const QString &name, const QString &path);
    QStandardItem *findTableItem(K2QtTableModel *view_model);
    QList<K2QtTableModel*> getTableList() const;
    void removeItem(QStandardItem *item);
private slots:
    void popupContextMenu(const QPoint &point);
    void itemDoubleClicked(const QModelIndex &index);
    void modelModifiedStatusChanged(bool modified);
    void showDetails();
private:
    QStandardItemModel *m_model;
    QMenu *m_context_menu;
    std::map<std::string,QAction*> m_actions;
};




#endif // PROJECTVIEW_HPP
