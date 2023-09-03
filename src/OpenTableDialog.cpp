#include "OpenTableDialog.hpp"
#include "ui_OpenTableDialog.h"

#include <QCompleter>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QScreen>                  // for point size to pixels

#include <kmt/TableIO.hpp>
#include <kmt/ErrorHandler.hpp>

#include "CustomErrMsgBox.hpp"
#include "ThemeHandler.hpp"
#include "rwview/TablePreview.hpp"
#include "rwview/ViewReadWrite.hpp"

#include "misc/KDefines.h"
#include "misc/SwitchLogHandlerHelper_.hpp"





OpenTableDialog::OpenTableDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::OpenTableDialog),
    m_table(nullptr)
{
    m_ui->setupUi(this);
    QFileSystemModel *model = new QFileSystemModel(m_ui->le_file_path);
    model->setReadOnly(true);
    model->setRootPath("D:");
    model->setOption(QFileSystemModel::DontWatchForChanges);
    m_ui->le_file_path->setCompleter(new QCompleter(model,m_ui->le_file_path));
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(3);
    m_model->setHorizontalHeaderLabels({tr("Name"), tr("Display Name"), tr("DataType")});
    m_ui->table_view->setModel(m_model);
    connect(m_ui->le_file_path, &QLineEdit::textChanged,this,&OpenTableDialog::showPreview);
    connect(m_ui->btn_browse, &QPushButton::clicked, this, &OpenTableDialog::browsePath);
    connect(m_ui->btn_open, &QPushButton::clicked, this, &OpenTableDialog::onOpen);
}

QString OpenTableDialog::getPath() const
{
    return result() == Accepted ? QFileInfo(m_ui->le_file_path->text()).path() : QString();
}

QString OpenTableDialog::tableName() const
{
    return result() == Accepted ? QFileInfo(m_ui->le_file_path->text()).fileName() : QString();
}

OpenTableDialog::~OpenTableDialog()
{
    delete m_ui;
}



void OpenTableDialog::showPreview(const QString &file)
{
    TablePreview preview(file);
    if(!preview.isValid())
    {
        m_ui->lb_table_name->setText(QString());
        m_ui->lb_row_count->setText(QString());
        m_ui->lb_column_count->setText(QString());
        m_ui->lb_sorting_order->setText(QString());
        m_ui->lb_sorting_order_icon->setPixmap(QPixmap());
        m_model->setRowCount(0);
        return;
    }


    m_ui->lb_table_name->setText(K_SQ_STR(preview.getTableName()));
    int image_size = (m_ui->lb_sorting_order->font().pointSize() / 72.0)
            * qApp->primaryScreen()->physicalDotsPerInch();
    if(preview.getSortingOrder() == km::SortingOrder::ASCENDING)
    {
        m_ui->lb_sorting_order->setText(tr("Ascending"));
        m_ui->lb_sorting_order_icon->setPixmap(icons::getIcon("ascending").pixmap(QSize(image_size,image_size)));
    }
    else
    {
        m_ui->lb_sorting_order->setText(tr("Descending"));
        m_ui->lb_sorting_order_icon->setPixmap(icons::getIcon("descending").pixmap(QSize(image_size,image_size)));
    }

    m_model->setRowCount(preview.getColumnCount());

    const auto &columns = preview.getColumns();
    for(km::IndexType i = 0; i < preview.getColumnCount(); ++i)
    {
        QStandardItem *cname = new QStandardItem(K_SQ_STR(columns[i].column_name));
        cname->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        QStandardItem *dname = new QStandardItem(K_SQ_STR(columns[i].display_name));
        dname->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        QStandardItem *dtype = new QStandardItem(km::dataTypeToString(columns[i].data_type));
        dtype->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

        m_model->setItem(i,0,cname);
        m_model->setItem(i,1,dname);
        m_model->setItem(i,2,dtype);
    }

    m_ui->lb_row_count->setText(QString::number(preview.getRowCount()));
    m_ui->lb_column_count->setText(QString::number(preview.getColumnCount()));


    QFile f(QFileInfo(file).path() + "/Views.xml");
    if(f.open(QFile::ReadOnly))
    {
        QString view_contents = f.readAll();
        f.close();
        int pos = 0, count = 0, len = 6 /*length of "<View "*/;
        while((pos = view_contents.indexOf("<View ",pos + len)) != -1) {++count;}
        m_ui->lb_view_count->setText(QString::number(count));
    }
}

void OpenTableDialog::browsePath()
{
    QString file_name = QFileDialog::getOpenFileName(this,
                                                     tr("Open Existing Table"),
                                                     QString(),
                                                     tr("Table Files (*.kmt)"));
    if(!file_name.isEmpty())
        m_ui->le_file_path->setText(file_name);
}

void OpenTableDialog::onOpen()
{
    // if it is empty that means current file path is invalid.
    if(m_ui->lb_table_name->text().isEmpty())
        return;
    QFileInfo info(m_ui->le_file_path->text());
    auto hdlr = km::err::getErrorHandler();
    QString error_message;
    SwitchLogHandlerHelper_ locker([&error_message](const std::string &logs){
        error_message = K_SQ_STR(logs);
    });

    m_table = km::readTableFrom(info.fileName().toStdString(),info.path().toStdString());

    if(!m_table)
    {
        CustomErrMsgBox::show(this, "Table Error", error_message);
        return;
    }
    if(!loadViews(m_table, (info.path() + "/Views.xml").toStdString().c_str(),m_views))
    {
        CustomErrMsgBox::show(this, "Views Error", error_message);
    }
    accept();
}


