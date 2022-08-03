#include "AutoValueGeneratorSettingsWindow.hpp"
#include "ui_AutoValueGeneratorSettingsWindow.h"


#include <QComboBox>
#include <QDateEdit>
#include <QLineEdit>

#include "ManipulatorHelperFncs_.hpp"
#include "../misc/KDefines.h"
#include "../K2QtTableModel.hpp"
#include "../LineEdit.hpp"
#include "../KmtFuncPopupEditor.hpp"
#include "../misc/SwitchLogHandlerHelper_.hpp"
#include "../CustomErrMsgBox.hpp"
#include "../IconProvider.hpp"


#define COLUMN_NAME 0
#define COLUMN_DATA_TYPE 1
#define COLUMN_DISP_NAME 2
#define COLUMN_FILL_TYPE 3
#define COLUMN_FILL_WITH 4
#define COLUMN_STATUS 5

AutoValueGeneratorSettingsWindow::AutoValueGeneratorSettingsWindow(DataEvaluator *evaluator, AddRowWidget *parent, bool as_dialog)
    : QDialog(parent),
      m_ui(new Ui::AutoValueGeneratorSettingsWindow),
      m_table(evaluator->getTable()),
      m_eval_data(evaluator->getEvalData()),
      m_evaluator(evaluator)
{
    m_ui->setupUi(this);
    setWindowFlag(as_dialog ? Qt::Dialog : Qt::Widget);

    m_ui->lb_warning_icon->setPixmap(QPixmap(":/icons/warning.png").scaled(QSize(16,16)));

    m_status_list.resize(m_eval_data.size(), true);
    QWidgetList ef_ignore_list;
    for(km::IndexType i = 0, count = m_eval_data.size(); i < count; ++i)
    {
        addColumn(i);
        if(auto le = dynamic_cast<LineEdit*>(m_ui->table_widget->cellWidget(i,COLUMN_FILL_WITH)))
            ef_ignore_list.append(le);
    }

    m_popup_editor = new KmtFuncPopupEditor(this);
    m_popup_editor->installEventFilterOnAllChildren(this);
    m_popup_editor->ignoreEventsOf(ef_ignore_list);

    connect(m_popup_editor, &KmtFuncPopupEditor::popupHidden, this, &AutoValueGeneratorSettingsWindow::popupEditorHidden);
    connect(m_ui->btn_apply, &QPushButton::clicked, this, &AutoValueGeneratorSettingsWindow::applySettings);
}


/**
 * @b index will be treated as row index in m_ui->table_widget and as column index for m_table.
 */
void AutoValueGeneratorSettingsWindow::addColumn(int index)
{
    const km::ColumnMetaData &column_meta = m_evaluator->getColumnMetaData(index);
    m_ui->table_widget->insertRow(index);

    // column name
    QTableWidgetItem *column_name = new QTableWidgetItem();
    column_name->setText(column_meta.column_name.c_str());
    m_ui->table_widget->setItem(index,COLUMN_NAME,column_name);

    // data type
    QTableWidgetItem *data_type = new QTableWidgetItem();
    data_type->setText(km::dataTypeToString(column_meta.data_type));
    m_ui->table_widget->setItem(index,COLUMN_DATA_TYPE,data_type);

    // display name
    QTableWidgetItem *display_name = new QTableWidgetItem();
    display_name->setText(column_meta.display_name.c_str());
    m_ui->table_widget->setItem(index,COLUMN_DISP_NAME,display_name);

    // fill type
    QComboBox *fill_type = new QComboBox(this);
    fill_type->addItems({tr("Value"), tr("Formula")});
    if(static_cast<bool>(column_meta.data_type & (km::DataType::INT32|km::DataType::INT64)))
        fill_type->addItem(tr("Auto Increament"));
    fill_type->setCurrentIndex(m_eval_data[index].fill_type);
    connect(fill_type, &QComboBox::currentIndexChanged, this, &AutoValueGeneratorSettingsWindow::fillTypeChanged);
    m_ui->table_widget->setCellWidget(index, COLUMN_FILL_TYPE, fill_type);

    // fill with
    QWidget *fill_with = nullptr;
    if(m_eval_data[index].fill_type == DataEvaluator::EvalData_::EXPR)
    {
        fill_with = new LineEdit(m_eval_data[index].expr, this);
        fill_with->setProperty("row-index", index);
        connect(dynamic_cast<LineEdit*>(fill_with), &LineEdit::focusGained, this, &AutoValueGeneratorSettingsWindow::showFormulaEditor);
    }
    else if(m_eval_data[index].fill_type == DataEvaluator::EvalData_::VALUE)
    {
        fill_with = createInputWidget(column_meta.data_type, m_eval_data[index].value, this);
    }
    //else {/*auto increament, do nothing*/}
    m_ui->table_widget->setCellWidget(index, COLUMN_FILL_WITH, fill_with);

    // status
    QTableWidgetItem *status = new QTableWidgetItem("No Issues");
    status->setData(Qt::DecorationRole, icons::getIcon("right"));
    status->setToolTip(tr("No problem in this column!"));
    m_ui->table_widget->setItem(index, COLUMN_STATUS, status);
    m_status_list[index] = true;
}

void AutoValueGeneratorSettingsWindow::fillTypeChanged(int index)
{
    QComboBox *fill_type = dynamic_cast<QComboBox*>(sender());
    K_RETURN_IF_NOT(fill_type);

    int row_index = 0;
    const int count = m_ui->table_widget->rowCount();
    while(row_index < count && fill_type != m_ui->table_widget->cellWidget(row_index,COLUMN_FILL_TYPE))
        ++row_index;
    K_RETURN_IF(row_index == count);

    //delete the current widget
    m_ui->table_widget->removeCellWidget(row_index,COLUMN_FILL_WITH);

    km::DataType data_type = m_evaluator->getColumnMetaData(row_index).data_type;
    if(index == 0)      //fill with value
    {
        auto w = createInputWidget(data_type,createDefaultValueFor(data_type),this);
        w->installEventFilter(m_popup_editor);
        m_ui->table_widget->setCellWidget( row_index, COLUMN_FILL_WITH, w);
    }
    else if(index == 1)     //generate value using formula
    {
        LineEdit *formula_editor = new LineEdit(m_eval_data[row_index].expr ,this);
        formula_editor->setProperty("row-index",row_index);
        formula_editor->setPlaceholderText(tr("Write your formula here!"));
        connect(formula_editor, &LineEdit::focusGained, this, &AutoValueGeneratorSettingsWindow::showFormulaEditor);
        m_ui->table_widget->setCellWidget(row_index, COLUMN_FILL_WITH, formula_editor);
    }

    auto status_item = m_ui->table_widget->item(row_index, COLUMN_STATUS);
    if(index == 0 || index == 2 || !m_eval_data[row_index].expr.trimmed().isEmpty())
    {
        m_status_list[row_index] = true;
        status_item->setText("No Issues");
        status_item->setData(Qt::DecorationRole, icons::getIcon("right"));
    }
    else
    {
        m_status_list[row_index] = false;
        status_item->setText("Has Issue");
        status_item->setData(Qt::DecorationRole, icons::getIcon("wrong"));
    }

    emit statusChanged();
}

void AutoValueGeneratorSettingsWindow::showFormulaEditor()
{
    LineEdit *new_line_edit = dynamic_cast<LineEdit*>(sender());
    K_RETURN_IF_NOT(new_line_edit);

    // if it was editing another line edit and then this line edit got focus.
    if(m_popup_editor->isVisible())
        m_popup_editor->hidePopup();

    // disconnect old signal slot connection
    disconnect(m_popup_editor, &KmtFuncPopupEditor::textShouldBeSaved, nullptr, nullptr);

    int row_index = new_line_edit->property("row-index").toInt();
    m_popup_editor->setText(new_line_edit->text());
    m_popup_editor->setProperty("row-index", row_index);

    // connect the signal to current line edit.
    connect(m_popup_editor, &KmtFuncPopupEditor::textShouldBeSaved, new_line_edit, &LineEdit::setText);

    m_popup_editor->setAbsTable(m_table, m_evaluator->getColumnMetaData(row_index).data_type);
    m_popup_editor->popup(new_line_edit, 4);
}


void AutoValueGeneratorSettingsWindow::popupEditorHidden()
{
    auto var = m_popup_editor->property("row-index");
    K_RETURN_IF_NOT(var.isValid());

    int row_index = var.toInt();
    auto status_item = m_ui->table_widget->item(row_index, COLUMN_STATUS);

    /* call explicitly so if text was changed and popup hidden in less than predefined time
     * it will be verified.*/
    m_popup_editor->checkFormula();

    if(m_popup_editor->isFormulaVerified())
    {
        m_eval_data[row_index].expr = m_popup_editor->getExpr();
        m_eval_data[row_index].compiled_token = m_popup_editor->getCompiledTokens();
        m_status_list[row_index] = true;
        status_item->setText("No Issues");
        status_item->setToolTip(tr("No problem in this column!"));
        status_item->setData(Qt::DecorationRole, icons::getIcon("right"));
    }
    else
    {
        m_eval_data[row_index].expr.clear();
        m_eval_data[row_index].compiled_token.clear();
        m_status_list[row_index] = false;
        status_item->setText("Has Issue");
        status_item->setToolTip(tr("Problem : ") + m_popup_editor->getErrorString());
        status_item->setData(Qt::DecorationRole, icons::getIcon("wrong"));
    }

    emit statusChanged();
}


void AutoValueGeneratorSettingsWindow::applySettings()
{
    for(int i = 0, count = m_ui->table_widget->rowCount(); i < count; ++i)
    {
        QComboBox *fill_type = dynamic_cast<QComboBox*>(m_ui->table_widget->cellWidget(i,COLUMN_FILL_TYPE));
        m_eval_data[i].fill_type = static_cast<DataEvaluator::EvalData_::FillType_>(fill_type->currentIndex());

        if(fill_type->currentIndex() == 0)  //fill with value
        {
            QString err;
            m_eval_data[i].value = m_eval_data[i].getter(m_ui->table_widget->cellWidget(i,COLUMN_FILL_WITH), &err);
            if(!err.isEmpty())
                m_eval_data[i].value = createDefaultVarValueFor(m_evaluator->getColumnMetaData(i).data_type);
        }
        // fill using formula is already set
        // for auto increase no need to do anything
    }

    m_evaluator->setEvalData(m_eval_data);
    accept();
}

void AutoValueGeneratorSettingsWindow::statusChanged()
{
    bool has_false_value = std::any_of(m_status_list.begin(), m_status_list.end(), [](bool s){
        return !s;
    });
    m_ui->btn_apply->setEnabled(!has_false_value);
}

