#include "AddRowWidget.hpp"
#include "AutoValueGeneratorSettingsWindow.hpp"
#include "DataEvaluator.hpp"
#include "ui_AddRowWidget.h"

#include "../K2QtTableModel.hpp"
#include "ManipulatorHelperFncs_.hpp"
#include "../misc/KDefines.h"
#include "../ThemeHandler2.hpp"
#include <QComboBox>
#include <QDateEdit>
#include <QFormLayout>
#include <QLineEdit>
#include <QToolTip>
#include <QScreen>

AddRowWidget::AddRowWidget(K2QtTableModel *model, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::AddRowWidget),
    m_model(model)
{
    m_ui->setupUi(this);

    m_ui->frame_widget->setMaximumWidth(qApp->primaryScreen()->availableSize().width() * 0.5);

    m_ui->btn_show_settings->setIcon(icons::getIcon("settings"));
    //create a form layout and add all nessessary information
    m_form_layout = new QFormLayout();

    auto column_info = m_model->getColumnInfo();
    QList<DataEvaluator::EvalData_> evaluator_data_list;

    for(std::size_t i = 0, c_count = column_info.size(); i < c_count; ++i)
    {
        auto column_name = K_SQ_STR(column_info[i].first);
        auto data_type = column_info[i].second;
        QWidget *editor_widget = createInputWidget(data_type, createDefaultValueFor(data_type), this);
        editor_widget->setProperty("column-index", int(i));
        m_form_layout->addRow(column_name,editor_widget);
        m_converters.push_back({getConvFuncFor(data_type),editor_widget});

        DataEvaluator::EvalData_ eval_data;
        eval_data.field = m_converters.back().second;
        eval_data.getter = m_converters.back().first;
        eval_data.fill_type = DataEvaluator::EvalData_::VALUE;
        evaluator_data_list.append(eval_data);
    }

    m_evaluator = new DataEvaluator(this);
    //m_evaluator->setColumnDetails(model,evaluator_data_list);
    m_ui->main_area->setLayout(m_form_layout);



    connectInputWidgets();
    //connect signals and slots
    connect(m_ui->btn_add_row, &QPushButton::clicked, this, &AddRowWidget::onAddRowClicked);
    connect(this, &AddRowWidget::rowInsertRequested, m_model, &K2QtTableModel::addRow);
    connect(model, &K2QtTableModel::newColumnAdded, this, &AddRowWidget::newColumnAdded);
    connect(m_ui->btn_show_settings, &QPushButton::clicked, this, &AddRowWidget::showSettings);
}

AddRowWidget::~AddRowWidget()
{
    //dirty hack as this will fetch and update the eval data list from the fields before saving it.
    m_evaluator->getEvalData();
    delete m_ui;
}

void AddRowWidget::onAddRowClicked()
{
    std::vector<km::Variant> elements;
    QString error;
    for(int i = 0, column_count = m_converters.size(); i < column_count; ++i)
    {
        elements.push_back(m_converters[i].first(m_converters[i].second,&error));
        if(!error.isEmpty())
        {
            QToolTip::showText(m_converters[i].second->mapToGlobal(QPoint(0,0)), error);
            break;
        }
    }
    if(error.isEmpty())
    {
        emit rowInsertRequested(elements);
        m_evaluator->autoIncreamentNeeded();
    }
}

void AddRowWidget::newColumnAdded(km::IndexType /*index*/, const km::ColumnMetaData &column_meta, int fill_type, const km::Variant &value, const QString &expr)
{
    using FillType_ = DataEvaluator::EvalData_::FillType_;
    QWidget *editor_widget = createInputWidget(column_meta.data_type, QString(), this);
    m_form_layout->addRow(K_SQ_STR(column_meta.display_name),editor_widget);
    m_converters.push_back({getConvFuncFor(column_meta.data_type),editor_widget});

    DataEvaluator::EvalData_ eval_data;
    eval_data.field = editor_widget;
    eval_data.fill_type = static_cast<FillType_>(fill_type);
    eval_data.getter = getConvFuncFor(column_meta.data_type);
    eval_data.expr = expr;
    eval_data.value = value;

    //tokens will be compiled automatically by DataEvaluator
    m_evaluator->newColumnAdded(column_meta, eval_data);
}

void AddRowWidget::connectInputWidgets()
{
    for(auto pair : m_converters)
    {
        if(auto le = dynamic_cast<QLineEdit*>(pair.second)) // covers int32, int64, float32, float64, string
        {
            connect(le, &QLineEdit::textEdited, this, &AddRowWidget::connectLineEdit);
        }
        else if(auto cb = dynamic_cast<QComboBox*>(pair.second))    // covers boolean
        {
            connect(cb, &QComboBox::currentIndexChanged, this, &AddRowWidget::connectComboBox);
        }
        else if(auto de = dynamic_cast<QDateEdit*>(pair.second))    // covers date
        {
            connect(de, &QDateEdit::dateChanged, this, &AddRowWidget::connectDateEdit);
        }
        else if(auto dt = dynamic_cast<QDateTimeEdit*>(pair.second))    // covers date_time
        {
            connect(dt, &QDateTimeEdit::dateTimeChanged, this, &AddRowWidget::connectDateTimeEdit);
        }
    }
}


void AddRowWidget::connectLineEdit(const QString&)
{
    auto sndr = sender();
    assert(sndr);
    inputValueChanged(dynamic_cast<QWidget*>(sndr));
}

void AddRowWidget::connectComboBox(int)
{
    auto sndr = sender();
    assert(sndr);
    inputValueChanged(dynamic_cast<QWidget*>(sndr));
}

void AddRowWidget::connectDateEdit(const QDate&)
{
    auto sndr = sender();
    assert(sndr);
    inputValueChanged(dynamic_cast<QWidget*>(sndr));
}

void AddRowWidget::connectDateTimeEdit(const QDateTime&)
{
    auto sndr = sender();
    assert(sndr);
    inputValueChanged(dynamic_cast<QWidget*>(sndr));
}

void AddRowWidget::showSettings()
{
    m_ui->btn_show_settings->setEnabled(false);
    AutoValueGeneratorSettingsWindow settings(m_evaluator,this,true);
    emit enableColumnModifiers(false);
    settings.show();
    connect(&settings, &AutoValueGeneratorSettingsWindow::accepted, this, [this]{
        m_ui->btn_show_settings->setEnabled(true);
        emit enableColumnModifiers(true);
    });
    connect(&settings, &AutoValueGeneratorSettingsWindow::rejected, this, [this]{
        m_ui->btn_show_settings->setEnabled(true);
        emit enableColumnModifiers(true);
    });
    settings.exec();
}

void AddRowWidget::inputValueChanged(QWidget *sndr)
{
    int column = sndr->property("column-index").toInt();
    assert(m_converters[column].second == sndr);

    km::Variant value = m_converters[column].first(sndr, nullptr);

    m_evaluator->valueChanged(column, value);
}

