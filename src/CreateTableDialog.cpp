#include "CreateTableDialog.hpp"

#include <tuple>

#include <QDir>
#include <QStringBuilder>
#include <QFormLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QLabel>
#include <QToolButton>

#include <kmt/ErrorHandler.hpp>

#include "KPairedLineEdit.hpp"
#include "SyntaxHighlighter.hpp"
#include "ThemeHandler2.hpp"
#include "CustomErrMsgBox.hpp"
#include "misc/ValidatorFactory.hpp"
#include "misc/KDefines.h"


static QLineEdit *cloneCellWidget(KPairedLineEdit *edit)
{
    auto temp = new KPairedLineEdit((QLineEdit*)nullptr, nullptr);

    if(auto item = edit->item())
        temp->setPair(item);
    else
        temp->setPair(edit->editor());
    temp->setText(edit->text());
    return temp;
}

static void swapColumnTableRow(QTableWidget *table, int index, int swap_with)
{
    // swap column name
    auto name_1 = cloneCellWidget(dynamic_cast<KPairedLineEdit*>(table->cellWidget(index, 0)));
    auto name_2 = cloneCellWidget(dynamic_cast<KPairedLineEdit*>(table->cellWidget(swap_with, 0)));
    table->setCellWidget(index, 0, name_2);
    table->setCellWidget(swap_with, 0, name_1);

    // swap column data type
    auto type_1 = dynamic_cast<QComboBox *>(table->cellWidget(index, 1));
    auto type_2 = dynamic_cast<QComboBox *>(table->cellWidget(swap_with, 1));
    int tmp = type_1->currentIndex();
    type_1->setCurrentIndex(type_2->currentIndex());
    type_2->setCurrentIndex(tmp);

    // swap column display name
    auto display_1 = table->takeItem(index, 2);
    auto display_2 = table->takeItem(swap_with, 2);
    table->setItem(index, 2, display_2);
    table->setItem(swap_with, 2, display_1);
}

CreateTableDialog::CreateTableDialog(QWidget *parent) : QDialog(parent), m_table(nullptr)
{
    setMinimumSize(620,480);
    setWindowTitle("Create New Table");
    setWindowIcon(icons::getIcon("table"));
    m_tname = new QLineEdit(this);

    m_sorting_order = new QComboBox(this);
    m_sorting_order->addItem(icons::getIcon("ascending"), tr("Ascending"));
    m_sorting_order->addItem(icons::getIcon("descending"), tr("Descending"));

    QHBoxLayout *path_layout = new QHBoxLayout();
    path_layout->setSpacing(0);
    m_tspath = new QLineEdit(QDir::homePath(), this);
    m_tspath->setProperty("-style-id","browse-edit");
    path_layout->addWidget(m_tspath);

    QToolButton *browse_path = new QToolButton(this);
    //
    browse_path->setIcon(icons::getIcon("browse"));
    browse_path->setIconSize(QSize(16,16));
    browse_path->setProperty("-style-id","browse-btn");
    browse_path->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored);
    connect(browse_path, &QToolButton::clicked, this, &CreateTableDialog::browsePath);
    path_layout->addWidget(browse_path);

    QGridLayout *grid_layout = new QGridLayout();
    grid_layout->addWidget(new QLabel(tr("Table Name"),this),0,0,1,1);
    grid_layout->addWidget(m_tname,0,1,1,1);
    grid_layout->addWidget(new QLabel(tr("Sorting Order"),this),0,2,1,1);
    grid_layout->addWidget(m_sorting_order,0,3,1,1);
    grid_layout->addWidget(new QLabel(tr("Path"),this),1,0,1,1);
    grid_layout->addLayout(path_layout,1,1,1,3);

    QVBoxLayout *table_layout = new QVBoxLayout();

    m_column_table = new QTableWidget(0, 3 /*4*/, this);
    QTableWidgetItem *hdr1 = new QTableWidgetItem(tr("Column Name"));
    hdr1->setToolTip(tr("Name of the column"));
    m_column_table->setHorizontalHeaderItem(0, hdr1);
    QTableWidgetItem *hdr2 = new QTableWidgetItem(tr("Data Type"));
    hdr2->setToolTip(tr("Data type of the column"));
    m_column_table->setHorizontalHeaderItem(1, hdr2);
    QTableWidgetItem *hdr3 = new QTableWidgetItem(tr("Display Name"));
    hdr3->setToolTip(tr("Display name will be used in entry form and for some other functionalities"));
    m_column_table->setHorizontalHeaderItem(2, hdr3);

    // selection mode and behavior affair
    m_column_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_column_table->setSelectionBehavior(QAbstractItemView::SelectRows);

    insertColumn();

    dynamic_cast<KPairedLineEdit*>(m_column_table->cellWidget(0, 0))->setText("ID");
    m_column_table->item(0, 2)->setText("ID Number");
    table_layout->addWidget(m_column_table);

    QToolButton *add_column = new QToolButton(this);
    add_column->setIcon(icons::getIcon("add"));
    connect(add_column, &QToolButton::clicked, this, &CreateTableDialog::insertColumn);
    QToolButton *remove_column = new QToolButton(this);
    remove_column->setIcon(icons::getIcon("remove"));
    connect(remove_column, &QToolButton::clicked, this, &CreateTableDialog::removeColumn);
    QToolButton *up_column = new QToolButton(this);
    up_column->setIcon(icons::getIcon("arrow-up"));
    connect(up_column, &QToolButton::clicked, this, &CreateTableDialog::moveUpColumn);
    QToolButton *down_column = new QToolButton(this);
    down_column->setIcon(icons::getIcon("arrow-down"));
    connect(down_column, &QPushButton::clicked, this, &CreateTableDialog::moveDownColumn);

    QHBoxLayout *table_btn_layout = new QHBoxLayout();
    table_btn_layout->addStretch();
    table_btn_layout->addWidget(add_column);
    table_btn_layout->addWidget(remove_column);
    table_btn_layout->addWidget(up_column);
    table_btn_layout->addWidget(down_column);
    table_btn_layout->addStretch();
    table_layout->addLayout(table_btn_layout);

    QPushButton *m_create_btn = new QPushButton(tr("Create"), this);
    connect(m_create_btn, &QPushButton::clicked, this, &CreateTableDialog::createTable);
    QPushButton *m_cancel_btn = new QPushButton(tr("Cancel"), this);
    connect(m_cancel_btn, &QPushButton::clicked, this, &CreateTableDialog::reject);

    QHBoxLayout *btn_layout = new QHBoxLayout();
    btn_layout->addStretch();
    btn_layout->addWidget(m_create_btn);
    btn_layout->addWidget(m_cancel_btn);

    QVBoxLayout *main_layout = new QVBoxLayout();
    main_layout->addLayout(grid_layout);
    main_layout->addLayout(table_layout);
    main_layout->addLayout(btn_layout);
    setLayout(main_layout);
}

std::tuple<km::Table *, QString> CreateTableDialog::getTable(QWidget *parent)
{
    CreateTableDialog dlg(parent);
    dlg.setModal(true);
    dlg.exec();
    return std::make_tuple(dlg.getTable(), dlg.getPath());
}



void CreateTableDialog::insertColumn()
{
    int index = m_column_table->currentRow() + 1;
    m_column_table->insertRow(index);

    //column_name
    KPairedLineEdit *column_name = new KPairedLineEdit((QTableWidgetItem*)nullptr);
    column_name->setText(QString("column_%1%2").arg(m_column_table->rowCount()).arg(index));
    m_column_table->setCellWidget(index,0,column_name);

    //column type
    QComboBox *data_types = new QComboBox(this);
    data_types->addItems({"int32","int64","float32","float64","string","boolean","date", "date_time"});
    connect(data_types, &QComboBox::currentIndexChanged, this, &CreateTableDialog::datatypeChanged);
    m_column_table->setCellWidget(index, 1, data_types);

    //display name
    QTableWidgetItem *display_name = new QTableWidgetItem(QString("column_%1%2").arg(m_column_table->rowCount()).arg(index));
    display_name->setBackground(Qt::transparent);
    column_name->setPair(display_name);
    m_column_table->setItem(index, 2, display_name);

    m_column_table->selectRow(index);
}

void CreateTableDialog::removeColumn()
{
    int index = m_column_table->currentRow();
    if (index >= 0)
    {
        m_column_table->removeRow(index);
        const int row_count = m_column_table->rowCount();
        if (row_count > index || (row_count - 1) == (--index))
            m_column_table->selectRow(index);
    }
}

void CreateTableDialog::moveUpColumn()
{
    int count = m_column_table->rowCount();
    if (count <= 1)
        return;
    int index = m_column_table->currentRow();
    int swap_with = (count + index - 1) % count;
    swapColumnTableRow(m_column_table, index, swap_with);
    m_column_table->selectRow(swap_with);
}

void CreateTableDialog::moveDownColumn()
{
    int count = m_column_table->rowCount();
    if (count <= 1)
        return;
    int index = m_column_table->currentRow();
    int swap_with = (index + 1) % count;
    swapColumnTableRow(m_column_table, index, swap_with);
    m_column_table->selectRow(swap_with);
}

void CreateTableDialog::createTable()
{
    QString error_string;
    std::string table_name = m_tname->text().trimmed().toStdString();
    std::vector<km::ColumnMetaData> column_vector;
    column_vector.reserve(m_column_table->rowCount());
    km::SortingOrder sorting_order;
    if (table_name.empty())
    {
        error_string = QStringLiteral("<span style=\"color:#ff0000;\">Table name field is empty!</span>");
    }
    else if (!QDir(m_tspath->text().trimmed()).exists())
    {
        error_string = QStringLiteral("<span style=\"color:#ff0000;\">Table save path doesn't exist</span>");
    }
    else if(QFile(getPath() % "/" % m_tname->text().trimmed() % ".kmt").exists())
    {
        error_string = QStringLiteral("<span style=\"color:#ff0000;\">Table already exists</span>");
    }
    else
    {
        //m_vec.clear();
        for (int i = 0, count = m_column_table->rowCount(); i < count; ++i)
        {
            column_vector.push_back({dynamic_cast<KPairedLineEdit*>(m_column_table->cellWidget(i,0))->text().toStdString(),
                                     m_column_table->item(i, 2)->text().toStdString(),
                                     static_cast<km::DataType>(uint16_t(1) << dynamic_cast<QComboBox *>(m_column_table->cellWidget(i, 1))->currentIndex())});
            //m_vec.push_back(dynamic_cast<QLineEdit*>(m_column_table->cellWidget(i,3))->text());
        }

        sorting_order = static_cast<km::SortingOrder>(m_sorting_order->currentIndex());
        auto hdlr = km::err::getErrorHandler();
        try
        {
            QDir(m_tspath->text().trimmed()).mkdir(m_tname->text().trimmed());
            km::err::setErrorHandler([&error_string](const std::string &msg)
                                     { error_string = K_SQ_STR(msg); });
            m_table = new km::Table(table_name, column_vector, sorting_order);
        }
        catch (...)
        {
            QDir(m_tspath->text().trimmed()).rmdir(m_tname->text().trimmed());
        }
        km::err::setErrorHandler(hdlr);
    }

    if (!error_string.isEmpty())
    {
        CustomErrMsgBox::showHtml(this, "Create Table", error_string);
    }
    else
    {
        emit accept();
    }
}

void CreateTableDialog::browsePath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     m_tspath->text(),
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!path.isEmpty())
        m_tspath->setText(path);
}

void CreateTableDialog::datatypeChanged(int /*index*/)
{
//    QComboBox *cb = qobject_cast<QComboBox *>(sender());
//    int pos = 0;
//    while (cb != m_column_table->cellWidget(pos, 1))
//        ++pos;
//    if (pos >= m_column_table->rowCount())
//        return;
//    QLineEdit *edit = qobject_cast<QLineEdit *>(m_column_table->cellWidget(pos, 3));
//    edit->setValidator(VldFactory::instance().validator(index));
//    if (!edit->hasAcceptableInput())
//        edit->clear();
}

QString CreateTableDialog::getPath() const
{
    return m_tspath->text().trimmed() % "/" % m_tname->text().trimmed();
}
