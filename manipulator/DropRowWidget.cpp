#include "DropRowWidget.hpp"
#include "ui_DropRowWidget.h"

#include <QScreen>
#include "../K2QtTableModel.hpp"

DropRowWidget::DropRowWidget(K2QtTableModel *model, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::DropRowWidget),
    m_model(model)
{
    m_ui->setupUi(this);
    m_ui->frame_widget->setMaximumWidth(qApp->primaryScreen()->availableSize().width() * 0.5);
    m_ui->sb_row_num->setMinimum(1);
    connect(m_ui->btn_drop_row, &QPushButton::clicked, this, &DropRowWidget::dropRow);
}

DropRowWidget::~DropRowWidget()
{
    delete m_ui;
}

void DropRowWidget::dropRow()
{
    m_model->dropRow(m_ui->sb_row_num->value() - 1);
}
