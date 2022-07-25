#ifndef ADDROWWIDGET_HPP
#define ADDROWWIDGET_HPP

#include <QDialog>

#include <kmt/Column.hpp>
#include <kmt/Table.hpp>


class K2QtTableModel;
class AutoValueGeneratorSettingsWindow;

class QFormLayout;

class DataEvaluator;
namespace Ui {
class AddRowWidget;
}

class AddRowWidget : public QWidget
{
    using ConvFunc_ = km::Variant(*)(QWidget*, QString*);
    Q_OBJECT
public:
    explicit AddRowWidget(K2QtTableModel *model/*, const std::vector<QString> &default_values*/, QWidget *parent = nullptr);
    K2QtTableModel *getModel() const;
    std::pair<ConvFunc_, QWidget *> getFieldWidget(km::IndexType column_index);
    ~AddRowWidget();
signals:
    void rowInsertRequested(const std::vector<km::Variant> &elements);
    void enableColumnModifiers(bool enable);
private slots:
    void onAddRowClicked();
    void on_pushButton_clicked();
    void inputValueChanged(QWidget *sndr);
public slots:
    void newColumnAdded(km::IndexType index, const km::ColumnMetaData &column_meta, int fill_type, const km::Variant &value, const QString &expr);
private:
    void connectInputWidgets();
    void connectLineEdit(const QString&);
    void connectComboBox(int);
    void connectDateEdit(const QDate&);
    void connectDateTimeEdit(const QDateTime&);
private:
    Ui::AddRowWidget *m_ui;
    K2QtTableModel *m_model;
    QFormLayout *m_form_layout;
    std::vector <std::pair<ConvFunc_,QWidget*>> m_converters;
    DataEvaluator *m_evaluator;
};

inline K2QtTableModel *AddRowWidget::getModel() const
{
    return m_model;
}

inline std::pair<AddRowWidget::ConvFunc_, QWidget *> AddRowWidget::getFieldWidget(km::IndexType column_index)
{
    return m_converters[column_index];
}

#endif // ADDROWWIDGET_HPP
