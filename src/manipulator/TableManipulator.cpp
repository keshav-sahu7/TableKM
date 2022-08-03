#include "TableManipulator.hpp"
#include "ui_TableManipulator.h"

#include "AddRowWidget.hpp"
#include "AddColumnWidget.hpp"
#include "DropRowWidget.hpp"
#include "TransformColumnWidget.hpp"

#include "../IconProvider.hpp"

TableManipulator::TableManipulator(K2QtTableModel *model/*, const std::vector<QString> &default_values*/, QWidget *parent) :
    DockWidget(QString(),parent),
    m_ui(new Ui::TableManipulator)
{
    assert(model->getTableType() == TableType::TABLE);
    m_ui->setupUi(this);
    QDockWidget::setWindowTitle(tr("Table Manipulator  - ") + model->getName());
    auto add_row_widget = new AddRowWidget(model,this);
    m_ui->tab_widget->addTab(add_row_widget ,icons::getIcon("add_e"),tr("Insert Row"));
    auto drop_row_widget = new DropRowWidget(model,this);
    m_ui->tab_widget->addTab( drop_row_widget,icons::getIcon("remove_e"),tr("Drop Row"));
    auto add_column_widget = new AddColumnWidget(model,this);
    m_ui->tab_widget->addTab( add_column_widget,icons::getIcon("add_column"),tr("Add New Column"));
    auto transform_column_widget = new TransformColumnWidget(model, this);
    m_ui->tab_widget->addTab( transform_column_widget, icons::getIcon("transform"),tr("Transform Column"));

    connect(model, &K2QtTableModel::destroyed, this, &TableManipulator::deleteLater);
    connect(add_row_widget, &AddRowWidget::enableColumnModifiers, add_column_widget, &AddColumnWidget::setEnabled);
}

TableManipulator::~TableManipulator()
{
    delete m_ui;
}
