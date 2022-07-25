#include "ViewDialog.hpp"
#include "ui_ViewDialog.h"

#include <QCheckBox>
#include <QMessageBox>
#include <QStringView>

#include "CustomErrMsgBox.hpp"
#include "KErrorToHtml.hpp"
#include "KmtFuncPopupEditor.hpp"

#include "misc/KDefines.h"

#define K_SWAP_DATA(obj1, obj2, set, get)\
    {\
        auto tmp = obj1->get();\
        obj1->set(obj2->get());\
        obj2->set(tmp);\
    }\

ViewDialog::ViewDialog(km::AbstractTable *source, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ViewDialog),
    m_view(nullptr),
    m_source(source)
{
    m_ui->setupUi(this);

    //set source table name
    m_ui->lb_source->setText(K_SQ_STR(m_source->getDecoratedName()));

    //fill the column table
    m_ui->table_widget->setRowCount(m_source->columnCount());
    for(km::IndexType i = 0, size = m_source->columnCount(); i < size; ++i)
    {
        const km::ColumnMetaData &column = m_source->getColumnMetaData(i);

        QCheckBox *selected = new QCheckBox();
        selected->setChecked(true);
        m_ui->table_widget->setCellWidget(i,0,selected);

        QTableWidgetItem *name = new QTableWidgetItem(K_SQ_STR(column.column_name));
        name->setFlags(name->flags() & ~Qt::ItemIsEditable);
        m_ui->table_widget->setItem(i,1,name);

        QTableWidgetItem *data_type = new QTableWidgetItem(K_SQ_STR(km::dataTypeToString(column.data_type)));
        data_type->setFlags(data_type->flags() & ~Qt::ItemIsEditable);
        m_ui->table_widget->setItem(i,2,data_type);

        QTableWidgetItem *display_name = new QTableWidgetItem(K_SQ_STR(column.display_name));
        display_name->setFlags(display_name->flags() & ~Qt::ItemIsEditable);
        m_ui->table_widget->setItem(i,3,display_name);

        //add to combobox of "sort by" option.
        m_ui->cb_sort_by->addItem(K_SQ_STR(column.column_name));
    }

    m_popup_editor = new KmtFuncPopupEditor(this);
    m_popup_editor->installEventFilterOnAllChildren(this);
    m_popup_editor->ignoreEventsOf({m_ui->le_formula});
    m_popup_editor->setAbsTable(source,km::DataType::BOOLEAN);

    connect(m_ui->le_formula, &LineEdit::focusGained, this, &ViewDialog::popupEditor);
    connect(m_popup_editor, &KmtFuncPopupEditor::popupHidden, this, &ViewDialog::popupEditorLostFocus);
    connect(m_ui->btn_move_up, &QPushButton::clicked, this, &ViewDialog::columnMoveUp);
    connect(m_ui->btn_move_down, &QPushButton::clicked, this, &ViewDialog::columnMoveDown);
    connect(m_ui->btn_select_all, &QPushButton::clicked, this, &ViewDialog::selectAll);
    connect(m_ui->btn_deselect_all, &QPushButton::clicked, this, &ViewDialog::deselectAll);
    connect(m_ui->btn_invert_selection, &QPushButton::clicked, this, &ViewDialog::invertSelections);
    connect(m_ui->btn_create_view, &QPushButton::clicked, this, &ViewDialog::createView);
}

km::BasicView *ViewDialog::getView()
{
    auto view = m_view;
    m_view = nullptr;
    return view;
}

ViewDialog::~ViewDialog()
{
    if(m_view)
        delete m_view;
    delete m_ui;
}

void ViewDialog::columnMoveUp()
{
    int count = m_ui->table_widget->rowCount();
    if (count <= 1)
        return;
    int index = m_ui->table_widget->currentRow();
    K_RETURN_IF(index < 0 || index >= count);
    int swap_with = (count + index - 1) % count;
    swapRows(index,swap_with);
    m_ui->table_widget->selectRow(swap_with);
}

void ViewDialog::columnMoveDown()
{
    int count = m_ui->table_widget->rowCount();
    if (count <= 1)
        return;
    int index = m_ui->table_widget->currentRow();
    K_RETURN_IF(index < 0 || index >= count);
    int swap_with = (index + 1) % count;
    swapRows(index,swap_with);
    m_ui->table_widget->selectRow(swap_with);
}

void ViewDialog::selectAll()
{
    for(int i = 0, count = m_ui->table_widget->rowCount(); i < count; ++i)
    {
        dynamic_cast<QCheckBox*>(m_ui->table_widget->cellWidget(i,0))->setChecked(true);
    }
}

void ViewDialog::deselectAll()
{
    for(int i = 0, count = m_ui->table_widget->rowCount(); i < count; ++i)
    {
        dynamic_cast<QCheckBox*>(m_ui->table_widget->cellWidget(i,0))->setChecked(false);
    }
}

void ViewDialog::invertSelections()
{
    for(int i = 0, count = m_ui->table_widget->rowCount(); i < count; ++i)
    {
        QCheckBox *cbox = dynamic_cast<QCheckBox*>(m_ui->table_widget->cellWidget(i,0));
        cbox->setChecked(!cbox->isChecked());
    }
}

void ViewDialog::createView()
{
    const std::string view_name = m_ui->le_view_name->text().toStdString();
    std::vector<std::string> columns;
    for(int i = 0, count = m_ui->table_widget->rowCount(); i < count; ++i)
    {
        if(dynamic_cast<QCheckBox*>(m_ui->table_widget->cellWidget(i,0))->isChecked())
            columns.push_back(m_ui->table_widget->item(i,1)->text().toStdString());
    }
    const std::string exp = m_ui->le_formula->text().toStdString();
    const std::string sort_by = m_ui->cb_sort_by->currentText().toStdString();
    km::SortingOrder s_order = static_cast<km::SortingOrder>(m_ui->cb_sorting_order->currentIndex());

    auto hdlr = km::err::getErrorHandler();
    QString error_string;
    try
    {
        km::err::setErrorHandler([&error_string](const std::string &msg){
            error_string = K_SQ_STR(msg);
        });
        m_view = new km::BasicView(view_name, m_source, columns, exp, sort_by, s_order);
        km::err::setErrorHandler(hdlr);
        accept();
    }
    catch(...)
    {
        //formatToHtml(error_string);
        CustomErrMsgBox::show(this, "View Error", error_string);
        km::err::setErrorHandler(hdlr);
    }

}

void ViewDialog::popupEditor()
{
    auto point = this->mapFromGlobal(m_ui->le_formula->mapToGlobal(QPoint(0,0)));
    auto size = QSize(m_ui->le_formula->width(), m_popup_editor->heightForLines(8));
    m_popup_editor->popup(point,size);
}

void ViewDialog::popupEditorLostFocus()
{
    m_ui->le_formula->setText(m_popup_editor->getExpr().replace('\n',' ').replace('\t',' '));
}


void ViewDialog::swapRows(int i1, int i2)
{
    QCheckBox *selected_1 = dynamic_cast<QCheckBox*>(m_ui->table_widget->cellWidget(i1,0));
    QCheckBox *selected_2 = dynamic_cast<QCheckBox*>(m_ui->table_widget->cellWidget(i2,0));

    K_SWAP_DATA(selected_1,selected_2,setChecked, isChecked);


    for(int c = 1; c < 3; ++c)
    {
        auto item_1 = m_ui->table_widget->item(i1,c);
        auto item_2 = m_ui->table_widget->item(i2,c);
        K_SWAP_DATA(item_1, item_2, setText, text);
    }
}




