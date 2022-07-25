#include "TransformColumnWidget.hpp"
#include "ui_TransformColumnWidget.h"

#include <QIntValidator>
#include <kmt/Core.hpp>

#include "../K2QtTableModel.hpp"
#include "../misc/KDefines.h"
#include "../KmtFuncPopupEditor.hpp"

TransformColumnWidget::TransformColumnWidget(K2QtTableModel *model, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::TransformColumnWidget),
    m_model(model)
{
    m_ui->setupUi(this);

    m_ui->frame_widget->setMaximumWidth(qApp->primaryScreen()->availableSize().width() * 0.5);

    QStringList column_list;
    for(int i = 1, count = model->columnCount(); i < count; ++i)
        column_list << K_SQ_STR( model->getColumnMetaData(i).value().column_name);

    m_ui->cb_column_list->addItems(column_list);

    m_popup_editor = new KmtFuncPopupEditor(this);
    m_popup_editor->installEventFilterOnAllChildren(this);
    m_popup_editor->ignoreEventsOf({m_ui->le_formula_editor});
    m_popup_editor->setMaxHeight(8);

    auto validator = new QIntValidator(-99999999,+99999999,this);
    m_ui->le_init_value->setValidator(validator);

    if(model->columnCount() > 1)
    {
        km::DataType data_type = model->getColumnMetaData(1).value().data_type;
        m_ui->lb_data_type->setText(km::dataTypeToString(data_type));
        m_popup_editor->setAbsTable(m_model->getAbsTable(), data_type);
        bool is_integer = static_cast<bool>(data_type & (km::DataType::INT32 | km::DataType::INT64));

        m_ui->rb_auto_increament->setEnabled(is_integer);
    }
    else{
        m_ui->btn_transform->setEnabled(false);
        m_ui->frame_widget->setDisabled(true);
    }

    connect(m_popup_editor, &KmtFuncPopupEditor::textShouldBeSaved, m_ui->le_formula_editor, &LineEdit::setText);
    connect(m_ui->le_formula_editor, &LineEdit::focusGained, m_popup_editor, QOverload<>::of(&KmtFuncPopupEditor::popup));
    connect(m_ui->cb_column_list, &QComboBox::currentIndexChanged, this, &TransformColumnWidget::columnChanged);
    connect(m_ui->btn_transform, &QPushButton::clicked, this, &TransformColumnWidget::transformColumn);
    connect(m_model, &K2QtTableModel::newColumnAdded, this, &TransformColumnWidget::newColumnAdded);
}

TransformColumnWidget::~TransformColumnWidget()
{
    delete m_ui;
}

void TransformColumnWidget::transformColumn()
{
    if(m_ui->rb_expr->isChecked())
        m_model->transformColumnEF(m_ui->cb_column_list->currentIndex()+1, m_ui->le_formula_editor->text());
    else
        m_model->transformColumnAI(m_ui->cb_column_list->currentIndex()+1, m_ui->le_init_value->text().toInt());
}

void TransformColumnWidget::columnChanged(int index)
{
    if(index < 0)
    {
        m_ui->lb_data_type->clear();
        return;
    }
    auto data_type = m_model->getColumnMetaData(index+1).value().data_type;
    m_ui->lb_data_type->setText(km::dataTypeToString(data_type));
    m_popup_editor->setAbsTable(m_model->getAbsTable(), data_type);
    if(static_cast<bool>(data_type & (km::DataType::INT32 | km::DataType::INT64)))
    {
        m_ui->rb_auto_increament->setEnabled(true);
    }
    else
    {
        m_ui->rb_expr->setChecked(true);
        m_ui->rb_auto_increament->setEnabled(false);
    }
}

void TransformColumnWidget::newColumnAdded(km::IndexType column_index, const km::ColumnMetaData &column_meta, int, const km::Variant& , const QString &)
{
    if(m_model->columnCount() == 1) //primary key should be read-only
        return;
    m_ui->cb_column_list->insertItem(column_index, K_SQ_STR(column_meta.column_name));

    if(m_ui->cb_column_list->count() == 1)
    {
        m_ui->frame_widget->setEnabled(true);
        m_ui->btn_transform->setEnabled(true);
        columnChanged(0);
    }
}
