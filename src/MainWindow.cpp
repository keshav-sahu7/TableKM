#include "HelpPage.hpp"
#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include <QMenuBar>
#include <QMessageBox>
#include <QDebug>

#include "TableView.hpp"
#include "K2QtTableModel.hpp"
#include "AboutUs.hpp"
#include "CreateTableDialog.hpp"
#include "IconProvider.hpp"
#include "manipulator/TableManipulator.hpp"
#include "ProjectView.hpp"
#include "ViewDialog.hpp"
#include "OpenTableDialog.hpp"
#include "QueryDialog.hpp"
#include "WelcomePage.hpp"
#include "misc/KDefines.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    setMenu();
    installDockWidgets();
    QTabBar *tab_bar = m_ui->tab_widget->tabBar();
    tab_bar->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tab_bar, &QTabBar::customContextMenuRequested, this, &MainWindow::popupContextMenu);
    connect(m_ui->tab_widget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    setContextMenuPolicy(Qt::NoContextMenu);
    openWelcomePage();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}


void MainWindow::showAboutQtDialog()
{
    QMessageBox::aboutQt(this);
}



void MainWindow::showAboutUsDialog()
{
    AboutUs about_us(this);
    about_us.setModal(true);
    about_us.exec();
}

void MainWindow::onQuery()
{
    QueryDialog qr(m_project_view->getTableList(),this);
    connect(&qr,&QueryDialog::addView, this, &MainWindow::addView);
    qr.setModal(true);
    qr.exec();
}

void MainWindow::openExistingTable()
{
    OpenTableDialog dlg(this);
    if(dlg.exec() == OpenTableDialog::Accepted)
    {
        QString path = dlg.getPath();
        km::Table *table = dlg.getTable();
        if(m_project_view->findTableItem(K_SQ_STR(table->getName()),path)) //if already exists
            return;

        K2QtTableModel *t_model = addTable(table,path/*,std::vector<QString>(table->columnCount())*/);
        QStandardItem *root_table_item = m_project_view->findTableItem(K_SQ_STR(table->getName()),path);
        QList<km::AbstractView*> &views = dlg.getViews();

        for(km::AbstractView *view : views)
        {
            K2QtTableModel *parent_model;
            if(view->getSourceTable() == table)
            {
                parent_model = t_model;
            }
            else
            {
                parent_model = m_project_view->findItem(view->getSourceTable(),root_table_item)
                        ->data().value<K2QtTableModel*>();
            }
            K2QtTableModel *view_model = new K2QtTableModel(dynamic_cast<km::BasicView*>(view),path,parent_model);
            connect(view_model, &K2QtTableModel::modifiedStatusChanged, this, &MainWindow::modelModifiedStatusChanged);
            m_project_view->addView(view_model,parent_model);
        }
    }
}


void MainWindow::createNewTable()
{
    auto result = CreateTableDialog::getTable(this);
    km::Table *table = std::get<0>(result);
    const QString &save_path = std::get<1>(result);
    //std::vector<QString> default_values = std::get<2>(result);
    if(table){
        addTable(table,save_path/*,default_values*/);
    }
}



void MainWindow::createNewView()
{
    QAction *action = dynamic_cast<QAction*>(sender());
    K_RETURN_IF_NOT(action);

    K2QtTableModel *parent_model = action->data().value<K2QtTableModel*>();
    ViewDialog view_dlg(parent_model->getAbsTable(),this);
    view_dlg.setModal(true);
    if(view_dlg.exec())
    {
        km::BasicView *basic_view = view_dlg.getView();
        K2QtTableModel *view_model = new K2QtTableModel(basic_view, parent_model->getPath(), parent_model);
        connect(view_model, &K2QtTableModel::modifiedStatusChanged, this, &MainWindow::modelModifiedStatusChanged);

        TableView *table_wdgt = new TableView(m_ui->tab_widget);
        table_wdgt->setModel(view_model);
        m_ui->tab_widget->addTab(table_wdgt, icons::getIcon("view"), view_model->getName());
        m_ui->tab_widget->setCurrentWidget(table_wdgt);

        m_project_view->addView(view_model,parent_model);
    }
}

void MainWindow::openInEditor()
{
    QAction *action = dynamic_cast<QAction*>(sender());
    K_RETURN_IF_NOT(action);

    K2QtTableModel *model = action->data().value<K2QtTableModel*>();
    if(model->isOpened()){
        m_ui->tab_widget->setCurrentWidget(model->getViewWidget());
        return;
    }
    else{
        connect(model, &K2QtTableModel::modifiedStatusChanged, this, &MainWindow::modelModifiedStatusChanged);
        TableView *table_wdgt = new TableView(m_ui->tab_widget);
        const char* icon = model->getTableType() == TableType::TABLE ? "table" : "view";
        table_wdgt->setModel(model);
        m_ui->tab_widget->addTab(table_wdgt, icons::getIcon(icon),model->getName());
        m_ui->tab_widget->setCurrentWidget(table_wdgt);
    }
}

void MainWindow::closeEditor()
{
    QAction *action = dynamic_cast<QAction*>(sender());
    K_RETURN_IF_NOT(action);

    K2QtTableModel *model = action->data().value<K2QtTableModel*>();
    if(model && model->isOpened())
    {
        disconnect(model, &K2QtTableModel::modifiedStatusChanged, this, &MainWindow::modelModifiedStatusChanged);
        closeTab(m_ui->tab_widget->indexOf(model->getViewWidget()));
    }
}

void MainWindow::saveOpenedTable(K2QtTableModel *model)
{
    if(model && model->isModified())
        model->saveTable();
}

void MainWindow::saveOpenedTable()
{
    QAction *action = dynamic_cast<QAction*>(sender());
    K_RETURN_IF_NOT(action);

    K2QtTableModel *model = action->data().value<K2QtTableModel*>();
    saveOpenedTable(model);
}

void MainWindow::closeTab(int index)
{
    K_RETURN_IF(index >= m_ui->tab_widget->count() || index < 0);
    QWidget *widget = m_ui->tab_widget->widget(index);
    widget->deleteLater();
    m_ui->tab_widget->removeTab(index);
}

void MainWindow::addView(K2QtTableModel *model, K2QtTableModel *parent)
{
    model->setParent(parent);
    connect(model, &K2QtTableModel::modifiedStatusChanged, this, &MainWindow::modelModifiedStatusChanged);
    TableView *table_wdgt = new TableView(m_ui->tab_widget);
    table_wdgt->setModel(model);
    m_ui->tab_widget->addTab(table_wdgt, icons::getIcon("view"), model->getName());
    m_ui->tab_widget->setCurrentWidget(table_wdgt);
    m_project_view->addView(model,parent);
}

void MainWindow::openDocumentationPage()
{
    HelpPage *help_page = nullptr;
    for(int i = 0, count = m_ui->tab_widget->count(); i < count; ++i)
    {
        if(m_ui->tab_widget->widget(i)->objectName() == "HelpPage")
        {
            help_page = dynamic_cast<HelpPage*>(m_ui->tab_widget->widget(i));
            m_ui->tab_widget->setCurrentWidget(help_page);
            break;
        }
    }
    if(!help_page)
    {
        help_page = new HelpPage(this);
        m_ui->tab_widget->addTab(help_page,tr("Formulae Helper"));
        m_ui->tab_widget->setCurrentWidget(help_page);
    }
}

void MainWindow::openWelcomePage()
{
    WelcomePage *welcome_page = nullptr;
    for(int i = 0, count = m_ui->tab_widget->count(); i < count; ++i)
    {
        if(m_ui->tab_widget->widget(i)->objectName() == "WelcomePage")
        {
            welcome_page = dynamic_cast<WelcomePage*>(m_ui->tab_widget->widget(i));
            m_ui->tab_widget->setCurrentWidget(welcome_page);
            break;
        }
    }
    if(!welcome_page)
    {
        welcome_page = new WelcomePage(this);
        m_ui->tab_widget->addTab(welcome_page,tr("Welcome"));
        m_ui->tab_widget->setCurrentWidget(welcome_page);
    }
}

void MainWindow::addThirdPartyWidget(QWidget *tab, const QString &label, int index, const QIcon &icon)
{
    m_ui->tab_widget->insertTab(index, tab, icon, label);
}

void MainWindow::modelModifiedStatusChanged(bool modified)
{
    K2QtTableModel *model = dynamic_cast<K2QtTableModel*>(sender());
    K_RETURN_IF_NOT(model);
    int index = m_ui->tab_widget->indexOf(model->getViewWidget());
    if(index != -1)
    {
        QString name = model->getName();
        if(modified)
            name.prepend('*');
        m_ui->tab_widget->setTabText( index, name);
    }
}

/**
 * this will be called from QTabBar of QTabWidget
 */
void MainWindow::popupContextMenu(const QPoint &point)
{
    K_RETURN_IF(point.isNull());

    if(QTabBar *tab_bar = dynamic_cast<QTabBar*>(sender()))
    {
        int tab_index = tab_bar->tabAt(point);
        K_RETURN_IF(tab_index == -1);
        TableView *tablewidget = dynamic_cast<TableView*>(m_ui->tab_widget->widget(tab_index));
        K_RETURN_IF_NOT(tablewidget);
        auto model = tablewidget->getK2QtModel();
        K_RETURN_IF_NOT(model);
        K_RETURN_IF_NOT(model->getTableType() == TableType::TABLE);

        QMenu menu;
        QAction *ac_cview = menu.addAction(icons::getIcon("view"),tr("Create View"));
        ac_cview->setData(QVariant::fromValue(model));
        connect(ac_cview, &QAction::triggered, this, &MainWindow::createNewView);

        QAction *ac_save = menu.addAction(icons::getIcon("save"),tr("Save"));
        ac_save->setEnabled(model->isModified());
        ac_save->setData(QVariant::fromValue(model));
        connect(ac_save, &QAction::triggered, this, QOverload<>::of(&MainWindow::saveOpenedTable));

        menu.exec(tab_bar->mapToGlobal(point));
    }
}

void MainWindow::setMenu()
{
    connect(m_ui->a_new_table,&QAction::triggered, this, &MainWindow::createNewTable);
    connect(m_ui->a_open, &QAction::triggered, this, &MainWindow::openExistingTable);
    connect(m_ui->a_exit,&QAction::triggered, this, &MainWindow::close);
    connect(m_ui->a_run_query, &QAction::triggered, this, &MainWindow::onQuery);
    connect(m_ui->a_about_us,&QAction::triggered, this, &MainWindow::showAboutUsDialog);
    connect(m_ui->a_about_qt,&QAction::triggered, this, &MainWindow::showAboutQtDialog);
    connect(m_ui->a_help, &QAction::triggered, this, &MainWindow::openDocumentationPage);
    connect(m_ui->a_welcome, &QAction::triggered, this, &MainWindow::openWelcomePage);
}

void MainWindow::installDockWidgets()
{
    //project view
    DockWidget *dw = new DockWidget(tr("Project View"),this);
    m_project_view = new ProjectView(this,dw);
    dw->setWidget(m_project_view);
    addDockWidget(Qt::RightDockWidgetArea, dw);
    dw->addVisibilityActionInMenu(m_ui->menu_view, this);     //add action in view menu
}

K2QtTableModel *MainWindow::addTable(km::Table *table, const QString &save_path/*, const std::vector<QString> &default_values*/)
{
    K2QtTableModel *model = new K2QtTableModel(table, save_path, this);
    connect(model, &K2QtTableModel::modifiedStatusChanged, this, &MainWindow::modelModifiedStatusChanged);

    TableView *table_wdgt = new TableView(m_ui->tab_widget);
    m_ui->tab_widget->addTab(table_wdgt, icons::getIcon("table"), K_SQ_STR(table->getName()));
    m_ui->tab_widget->setCurrentWidget(table_wdgt);
    table_wdgt->setModel(model);

    //add to project view
    m_project_view->addTable(model);

    TableManipulator *manipulator = new TableManipulator(model, this);

    addDockWidget(Qt::BottomDockWidgetArea,manipulator);
    manipulator->addVisibilityActionInMenu(m_ui->menu_view, this);
    return model;
}
