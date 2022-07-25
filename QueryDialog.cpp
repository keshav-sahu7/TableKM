#include "CustomErrMsgBox.hpp"
#include "QueryDialog.hpp"
#include "ui_QueryDialog.h"

#include <fstream>

#include <QFileDialog>
#include <QFocusEvent>
#include <QInputDialog>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>

#include <kmt/CSVWriter.hpp>
#include <kmt/Printer.hpp>

#include "TableView.hpp"
#include "K2QtTableModel.hpp"
#include "misc/KDefines.h"
#include "SyntaxHighlighter.hpp"

#include "misc/SwitchLogHandlerHelper_.hpp"


QueryDialog::QueryDialog(QList<K2QtTableModel*> models, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::QueryDialog),
    m_current_parent(nullptr)
{
    m_ui->setupUi(this);

    m_table_view = new TableView(this);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_table_view);
    m_ui->table_container->setLayout(layout);

    m_ui->cb_table_list->addItem("");
    for(auto model : models)
    {
        m_ui->cb_table_list->addItem(model->getName(),QVariant::fromValue(model));
    }

    m_popup_editor = new KmtFuncPopupEditor(this);
    m_popup_editor->installEventFilterOnAllChildren(this);
    m_popup_editor->ignoreEventsOf({m_ui->le_filter_expr});

    connect(m_ui->le_filter_expr, &LineEdit::focusGained, m_popup_editor, QOverload<>::of(&KmtFuncPopupEditor::popup));
    connect(m_popup_editor, &KmtFuncPopupEditor::textShouldBeSaved, m_ui->le_filter_expr, &LineEdit::setText);
    connect(m_ui->cb_table_list, &QComboBox::currentIndexChanged, this, &QueryDialog::sourceTableChanged);
    connect(m_ui->btn_print, &QPushButton::clicked, this, &QueryDialog::printTable);
    connect(m_ui->btn_run_query, &QPushButton::clicked, this, &QueryDialog::runQuery);
    connect(m_ui->btn_as_view, &QPushButton::clicked,this, &QueryDialog::addAsView);
    connect(m_ui->btn_export_csv, &QPushButton::clicked, this, &QueryDialog::exportAsCSV);
    connect(m_ui->btn_export_text, &QPushButton::clicked, this, &QueryDialog::exportAsText);

}

QueryDialog::~QueryDialog()
{
    delete m_table_view->getK2QtModel();
    delete m_ui;
}

void QueryDialog::sourceTableChanged(int index)
{
    if(index < 1)
    {
        m_ui->cb_sort_by->clear();
        m_popup_editor->setAbsTable(nullptr, km::DataType::INT32);
        return;
    }
    QStringList user_list = m_ui->le_column_names->text().split(',');
    auto model = m_ui->cb_table_list->currentData().value<K2QtTableModel*>();

    QString old_sort_by = m_ui->cb_sort_by->currentText();
    m_ui->cb_sort_by->clear();
    for(auto str : user_list)
    {
        str = str.trimmed();
        if(model->getAbsTable()->findColumn(str.toStdString()))
            m_ui->cb_sort_by->addItem(str);
    }
    int curr_index = m_ui->cb_sort_by->findText(old_sort_by);
    m_ui->cb_sort_by->setCurrentIndex(curr_index == -1 ? 0 : curr_index);
    m_popup_editor->setAbsTable(model->getAbsTable(), km::DataType::BOOLEAN);
}

void QueryDialog::exportAsCSV()
{

    if(auto model = m_table_view->getK2QtModel())
    {
        QString filename = QFileDialog::getSaveFileName(this,tr("Export As CSV File"),QDir::homePath(),tr("CSV file (*.csv)"));
        if(!filename.isEmpty())
            km::writeAsCsv(model->getAbsTable(),filename.toStdString());
    }
}

void QueryDialog::exportAsText()
{
    if(auto model = m_table_view->getK2QtModel())
    {
        QString filename = QFileDialog::getSaveFileName(this,tr("Save As Text File"),QDir::homePath(),tr("Text file (*.txt);; All Files"));
        if(!filename.isEmpty())
        {
            km::Printer printer(model->getAbsTable());
            std::ofstream ofs(filename.toStdString());
            if(ofs.is_open())
            {
                printer.print(ofs,"\t|\t");
                ofs.close();
            }
        }
    }
}

void QueryDialog::printTable()
{
    K_RETURN_IF_NOT(m_table_view->getK2QtModel());

    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer,this);
    if(dialog.exec() == QDialog::Accepted)
    {
        QString html_str = "<!DOCTYPE html>"
                           "<html>"
                           "     <head>"
                           "         <style>"
                           "         th{"
                           "             background-color:green;"
                           "         }"
                           "         td{"
                           "             background-color:gray;"
                           "         }"
                           "         </style>"
                           "     </head>"
                           "     <body>"
                           "     <table border=\"1\" style=\"border-collapse:collapse;\">"
                           "     <thead>%1</thead>"
                           "     <tbody>%2</tbody>"
                           "     </table>"
                           "     </body>"
                           "</html>";

        auto model = m_table_view->getK2QtModel();

        auto column_count = model->columnCount();
        QString header_html = "<tr>";
        for(int i = 0; i < column_count; ++i)
        {
            QString s = K_SQ_STR(model->getColumnMetaData(i).value_or(km::ColumnMetaData{"",km::DataType::INT32}).display_name);
            header_html.append("<th>" + s + "</th>");
        }
        header_html.append("</tr>");

        QString element_html;
        for(int r = 0, row_count = model->rowCount(); r < row_count; ++r)
        {
            QString row = "<tr>";
            for(int c = 0; c < column_count; ++c)
            {
                row.append("<td>" + model->data(model->index(r,c),Qt::DisplayRole).toString() + "</td>");
            }
            row.append("</tr>");
            element_html.append(row);
        }

        html_str = html_str.arg(header_html).arg(element_html);
        QTextDocument document(this);
        document.setHtml(html_str);
        document.print(&printer);
    }
}

void QueryDialog::addAsView()
{
    K_RETURN_IF_NOT(m_current_parent);
    auto model = m_table_view->getK2QtModel();
    K_RETURN_IF_NOT(model);

    QString text = QInputDialog::getText(this, tr("Query Runner"), tr("Enter View Name")).trimmed();
    K_RETURN_IF(text.isEmpty());

    bool is_valid = !text.contains('\t');
    if(is_valid)
        is_valid = dynamic_cast<km::BasicView*>(model->getAbsTable())->setViewName(text.toStdString());
    if(is_valid)
    {
        m_table_view->setModel(nullptr);
        m_ui->lb_ncol->clear();
        m_ui->lb_nrow->clear();
        emit addView(model, m_current_parent);
    }
    else
    {
        CustomErrMsgBox::showHtml(
                    this,
                    tr("Invalid View Name"),
                    tr("<span style=\"color:red\">"
                       "View name should start with alphabet followed by any"
                       " character <b>except</b> \\ / and - and non English alphabets.</span>"));
    }
}

void QueryDialog::runQuery()
{
    K_RETURN_IF_NOT(preValidate());
    QString error;
    SwitchLogHandlerHelper_ ignore_this([&error](const std::string &str){
        error = K_SQ_STR(str);
    });
    try{
        std::vector<std::string> vec;
        QStringList user_list = m_ui->le_column_names->text().split(',');
        for(const auto &val : user_list)
        {
            vec.push_back(val.trimmed().toStdString());
        }

        K2QtTableModel *source_model = m_ui->cb_table_list->currentData().value<K2QtTableModel*>();
        km::BasicView *view = new km::BasicView(
                    "__query-runner",
                    source_model->getAbsTable(),
                    vec,
                    m_ui->le_filter_expr->text().toStdString(),
                    m_ui->cb_sort_by->currentText().toStdString(),
                    static_cast<km::SortingOrder>(m_ui->cb_sorting_order->currentIndex())
                    );
        K2QtTableModel *model = new K2QtTableModel(view,source_model->getPath(),nullptr);
        K2QtTableModel *old_model = m_table_view->getK2QtModel();
        m_table_view->setModel(model);
        delete old_model;
        m_current_parent = source_model;

        m_ui->lb_ncol->setText( QString::number(model->columnCount()));
        m_ui->lb_nrow->setText( QString::number(model->rowCount()));
    }
    catch(...){
        CustomErrMsgBox::show(this,tr("Query Runner"),error);
    }
}

bool QueryDialog::preValidate()
{
    QString error;
    if(m_ui->le_column_names->text().trimmed().isEmpty()){
        error = tr("Column names field is empty! Use * to select all columns.");
    }
    else if(m_ui->cb_table_list->currentIndex() < 1){
        error = tr("No table is selected!");
    }
    else if(m_ui->cb_sort_by->currentIndex() == -1)
    {
        error = tr("No sort by selected!");
    }
    if(!error.isEmpty())
    {
        CustomErrMsgBox::showHtml(this,tr("Query Runner"),QString("<span style=\"color=red\">%1</span>").arg(error));
        return false;
    }
    return true;
}




