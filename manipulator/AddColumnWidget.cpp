#include "AddColumnWidget.hpp"
#include "ui_AddColumnWidget.h"



#include <QPlainTextEdit>

#include "ManipulatorHelperFncs_.hpp"

#include "../KmtFuncPopupEditor.hpp"
#include "../LineEdit.hpp"
#include "../SyntaxHighlighter.hpp"
#include "../K2QtTableModel.hpp"
#include "../misc/KDefines.h"

AddColumnWidget::AddColumnWidget(K2QtTableModel *model, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::AddColumnWidget),
    m_model(model)
{
    m_ui->setupUi(this);

    m_ui->frame_widget->setMaximumWidth(qApp->primaryScreen()->availableSize().width() * 0.5);

    m_formula_field = new LineEdit(this);
    m_formula_field->setPlaceholderText(tr("Write your formula here!"));

    m_value_field = nullptr;

    m_ui->lyt_field->addWidget(m_formula_field);

    m_popup_editor = new KmtFuncPopupEditor(this);
    m_popup_editor->installEventFilterOnAllChildren(this);
    m_popup_editor->ignoreEventsOf({m_formula_field});
    m_popup_editor->setAbsTable(m_model->getAbsTable(), km::DataType::INT32);


    connect(m_formula_field, &LineEdit::focusGained, m_popup_editor, QOverload<>::of(&KmtFuncPopupEditor::popup));
    connect(m_popup_editor, &KmtFuncPopupEditor::textShouldBeSaved, m_formula_field, &LineEdit::setText);
    connect(m_ui->cb_fill_type, &QComboBox::currentIndexChanged, this, &AddColumnWidget::fillTypeChanged);
    connect(m_ui->cb_dtype, &QComboBox::currentIndexChanged, this, &AddColumnWidget::dataTypeChanged);
    connect(m_ui->btn_add_column, &QPushButton::clicked, this, &AddColumnWidget::addColumn);
}

AddColumnWidget::~AddColumnWidget()
{
    delete m_ui;
}

void AddColumnWidget::fillTypeChanged(int index)
{
    m_formula_field->setVisible(index == 1);       // 1 means "use formulae to generate value"

    switch(index)
    {
    case 0:
    {
        delete m_value_field;
        m_value_field = createInputWidget(km::toDataType(m_ui->cb_dtype->currentIndex()),QString(),this);
        m_ui->lyt_field->addWidget(m_value_field);
        break;
    }
    case 1:
    case 2:
    {
        delete m_value_field;
        m_value_field = nullptr;
        break;
    }
    }
}

void AddColumnWidget::dataTypeChanged(int index)
{
    int current_fill_type = m_ui->cb_fill_type->currentIndex();
    m_ui->cb_fill_type->clear();
    if(index == 0 || index == 1)    //int32 and int64
    {
        m_ui->cb_fill_type->addItems(
                    {
                        tr("Use Value"),
                        tr("Use Formulae To Generate Value"),
                        tr("Auto Increament")
                    });
    }
    else
    {
        m_ui->cb_fill_type->addItems(
                    {
                        tr("Use Value"),
                        tr("Use Formulae To Generate Value")
                    });
    }
    if(m_ui->cb_fill_type->count() > current_fill_type)
        m_ui->cb_fill_type->setCurrentIndex(current_fill_type);
    else
        m_ui->cb_fill_type->setCurrentIndex(0);

    m_popup_editor->setAbsTable(m_model->getAbsTable(), km::toDataType(m_ui->cb_dtype->currentIndex()));
}

void AddColumnWidget::addColumn()
{
    km::DataType data_type = km::toDataType(m_ui->cb_dtype->currentIndex());
    km::ColumnMetaData meta = km::ColumnMetaData(m_ui->le_cname->text().toStdString(),
                                                 m_ui->le_dname->text().toStdString(),
                                                 data_type);
    switch(m_ui->cb_fill_type->currentIndex())
    {
    case 0:
    {
        K_RETURN_IF_NOT(m_value_field);
        ConvFunc_ fnc = getConvFuncFor(data_type);
        QString error;
        km::Variant value = fnc(m_value_field,&error);
        K_RETURN_IF_NOT(error.isEmpty());
        m_model->addColumnFV(meta,value);
        break;
    }
    case 1:
    {
        m_model->addColumnEF(meta,m_formula_field->text());
        break;
    }
    case 2:
    {
        m_model->addColumnAI(meta);
    }
    }
}
