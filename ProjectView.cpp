#include "ProjectView.hpp"

#include "K2QtTableModel.hpp"
#include "IconProvider.hpp"
#include "MainWindow.hpp"
#include "TableInfoDialog.hpp"
#include "misc/KDefines.h"



ProjectView::ProjectView(MainWindow *main_window, QWidget *parent) : QTreeView(parent)
{
    m_model = new QStandardItemModel(this);
    m_model->setHorizontalHeaderLabels({"Tables"});
    setModel(m_model);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &ProjectView::customContextMenuRequested, this, &ProjectView::popupContextMenu);

    //create menu
    m_context_menu = new QMenu(this);

    QAction *ac_cview = m_context_menu->addAction(icons::getIcon("view"),tr("Create View"));
    connect(ac_cview, &QAction::triggered, main_window, &MainWindow::createNewView);
    m_actions.insert({"create-view",ac_cview});

    QAction *ac_open = m_context_menu->addAction(icons::getIcon("open"),tr("Open In Editor"));
    connect(ac_open, &QAction::triggered, main_window, &MainWindow::openInEditor);
    connect(this, &ProjectView::doubleClicked, this, &ProjectView::itemDoubleClicked);
    m_actions.insert({"open-in-editor",ac_open});

    QAction *ac_close = m_context_menu->addAction(icons::getIcon("close-editor"),tr("Close From Editor"));
    connect(ac_close, &QAction::triggered, main_window, &MainWindow::closeEditor);
    m_actions.insert({"close-from-editor",ac_close});

    QAction *ac_save = m_context_menu->addAction(icons::getIcon("save"),tr("Save"));
    connect(ac_save, &QAction::triggered, main_window, QOverload<>::of(&MainWindow::saveOpenedTable));
    m_actions.insert({"save",ac_save});

    QAction *ac_delete = m_context_menu->addAction(icons::getIcon("trash"),tr("Delete"));
    connect(ac_delete, &QAction::triggered, main_window, &MainWindow::deleteView);
    m_actions.insert({"delete-view",ac_delete});

    QAction *ac_details = m_context_menu->addAction(icons::getIcon("details"),tr("Show Details"));
    connect(ac_details, &QAction::triggered, this, &ProjectView::showDetails);
    m_actions.insert({"details",ac_details});

    QAction *ac_close_table = m_context_menu->addAction(icons::getIcon("close"), tr("Close Table"));
    connect(ac_close_table, &QAction::triggered, main_window, &MainWindow::deleteView);
    m_actions.insert({"close-table",ac_close_table});
}

QStandardItem *ProjectView::addTable(K2QtTableModel *table)
{
    const QString &table_name = table->getName();
    QStandardItem *item = new QStandardItem(icons::getIcon("table"),table_name);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    item->setData(QVariant::fromValue(table));
    m_model->invisibleRootItem()->appendRow(item);
    table->setProjectItem(item);
    connect(table, &K2QtTableModel::modifiedStatusChanged, this, &ProjectView::modelModifiedStatusChanged);
    return item;
}

QStandardItem *ProjectView::addView(K2QtTableModel *view, K2QtTableModel *parent)
{
    if(auto parent_item = parent->getProjectedItem())
    {
        const QString &view_name = view->getName();

        QStandardItem *view_item = new QStandardItem(icons::getIcon("view"), view_name);
        view_item->setData(QVariant::fromValue(view));
        view_item->setFlags(view_item->flags() & ~Qt::ItemIsEditable);
        parent_item->appendRow(view_item);

        view->setProjectItem(view_item);

        return view_item;
    }

    qWarning("Project View : Could not add view to table!");
    return nullptr;
}

void ProjectView::remove(K2QtTableModel *table)
{
    QStandardItem *item = table->getProjectedItem();
    if(item){
        item->parent()->removeRow(item->row());
    }
}

K2QtTableModel *ProjectView::modelAt(const QPoint &point)
{
    QModelIndex index = indexAt(point);
    if(!index.isValid())
        return nullptr;
    QVariant data = m_model->itemFromIndex(index)->data();
    return data.value<K2QtTableModel*>();
}



QStandardItem *ProjectView::findItem(km::AbstractTable *table, QStandardItem *root)
{
    if(!root)
    {
        root = m_model->invisibleRootItem();
    }
    for(int i = 0, count = root->rowCount(); i < count; ++i)
    {
        QStandardItem *child = root->child(i);
        if(child->data().value<K2QtTableModel*>()->getAbsTable() == table)
            return child;
        else if(QStandardItem *item = findItem(table,child))
            return item;
    }
    return nullptr;
}

QStandardItem *ProjectView::findTableItem(const QString &name, const QString &path)
{
    for(int i = 0; i < m_model->rowCount(); ++i)
    {
        QStandardItem *item = m_model->item(i);
        auto model = item->data().value<K2QtTableModel*>();
        if(model->getName() == name && model->getPath() == path)
            return item;
    }
    return nullptr;
}

QStandardItem *ProjectView::findTableItem(K2QtTableModel *view_model)
{
    if(view_model->getTableType() == TableType::TABLE)
        return findTableItem(view_model->getName(), view_model->getPath());
    km::AbstractTable *table = view_model->getAbsTable();
    while(true){
        km::AbstractView *source = dynamic_cast<km::AbstractView*>(table);
        if(!source)
            return findTableItem(K_SQ_STR(table->getName()), view_model->getPath());
        else
            table = source->getSourceTable();
    }
    // it shouldn't be reached.
    return nullptr;
}

QList<K2QtTableModel*> ProjectView::getTableList() const
{
    QList<K2QtTableModel*> list;
    for(int i = 0, count = m_model->invisibleRootItem()->rowCount(); i < count; ++i)
    {
        list.append(m_model->item(i)->data().value<K2QtTableModel*>());
    }
    return list;
}

void ProjectView::removeItem(QStandardItem *item)
{
    auto parent = item->parent();
    if(parent){
        parent->removeRow(item->row());
    }
    else{
        m_model->removeRow(item->row());
    }
}

void ProjectView::popupContextMenu(const QPoint &point)
{
    if(point.isNull())
        return;
    K2QtTableModel *model = modelAt(point);
    if(model){
        for(auto p : m_actions)
        {
            p.second->setData(QVariant::fromValue(model));
        }
        bool opened = model->isOpened();
        m_actions.at("open-in-editor")->setVisible(!opened);
        m_actions.at("close-from-editor")->setVisible(opened);
        m_actions.at("save")->setVisible(model->getTableType() == TableType::TABLE);
        m_actions.at("save")->setEnabled(model->isModified());
        m_actions.at("create-view")->setVisible(model->getTableType() == TableType::TABLE);
        m_actions.at("delete-view")->setVisible(model->getTableType() == TableType::BASIC_VIEW);
        m_actions.at("close-table")->setVisible(model->getTableType() == TableType::TABLE);
        m_context_menu->popup(mapToGlobal(point));
    }
}

void ProjectView::itemDoubleClicked(const QModelIndex &index)
{
    try{
        QAction *action = m_actions.at("open-in-editor");
        QStandardItem *item = m_model->itemFromIndex(index);
        K2QtTableModel *model = item->data().value<K2QtTableModel*>();
        action->setData(QVariant::fromValue(model));
        emit action->trigger();
    }
    catch(...){}
}

void ProjectView::modelModifiedStatusChanged(bool modified)
{
    K2QtTableModel *model = dynamic_cast<K2QtTableModel*>(sender());
    K_RETURN_IF_NOT(model);
    QStandardItem *item = model->getProjectedItem();
    K_RETURN_IF_NOT(item);
    QString name = model->getName();
    QFont ft = font();
    if(modified)
    {
        name.prepend('*');
    }
    item->setText(name);
    ft.setBold(modified);
    item->setFont(ft);
}

void ProjectView::showDetails()
{
    QAction *action = dynamic_cast<QAction*>(sender());
    K_RETURN_IF_NOT(action);
    K2QtTableModel *model = action->data().value<K2QtTableModel*>();
    K_RETURN_IF_NOT(model);
    TableInfoDialog::showDetails(model,this);
}
