#include "TableInfoDialog.hpp"
#include "ui_TableInfoDialog.h"

#include <kmt/AbstractView.hpp>

#include "K2QtTableModel.hpp"
#include "misc/KDefines.h"



TableInfoDialog::TableInfoDialog(K2QtTableModel *model, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::TableInfoDialog)
{
    m_ui->setupUi(this);
    m_ui->lb_name->setText(model->getName());
    if(model->getTableType() == TableType::TABLE)
    {
        m_ui->lb_type->setText("Table");
        m_ui->lb_fe->setHidden(true);
        m_ui->lb_fevalue->setHidden(true);
    }
    else{
        m_ui->lb_type->setText("View");
        if(km::AbstractView *view = dynamic_cast<km::AbstractView*>(model->getAbsTable()))
            m_ui->lb_fevalue->setText(K_SQ_STR(view->getFilterFormula()));
    }

    m_ui->lb_path->setText(model->getPath());
    if(auto item = model->getProjectedItem())
        m_ui->lb_ndep->setText(QString::number(item->rowCount()));
    else
        m_ui->lb_ndep->setText("0");
    m_ui->lb_norw->setText(QString::number(model->rowCount()));
    m_ui->lb_ncol->setText(QString::number(model->columnCount()));

    int column_count = model->columnCount();
    m_ui->column_table->setRowCount(column_count);
    for(int i = 0; i < column_count; ++i)
    {
        if(auto opt = model->getColumnMetaData(i))
        {
            km::ColumnMetaData meta = opt.value();
            QTableWidgetItem *item1 = new QTableWidgetItem(K_SQ_STR(meta.column_name));
            QTableWidgetItem *item2 = new QTableWidgetItem(K_SQ_STR(meta.display_name));
            QTableWidgetItem *item3 = new QTableWidgetItem(QLatin1String(km::dataTypeToString(meta.data_type)));

            item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
            item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
            item3->setFlags(item3->flags() & ~Qt::ItemIsEditable);

            m_ui->column_table->setItem(i,0,item1);
            m_ui->column_table->setItem(i,1,item2);
            m_ui->column_table->setItem(i,2,item3);
        }
    }
}

TableInfoDialog::~TableInfoDialog()
{
    delete m_ui;
}

void TableInfoDialog::showDetails(K2QtTableModel *model, QWidget *parent)
{
    K_RETURN_IF_NOT(model);
    TableInfoDialog dialog(model,parent);
    dialog.setModal(true);
    dialog.exec();
}
