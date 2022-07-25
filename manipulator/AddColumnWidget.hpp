#ifndef ADDCOLUMNWIDGET_HPP
#define ADDCOLUMNWIDGET_HPP

#include <QDialog>



class K2QtTableModel;

class LineEdit;

class KmtFuncPopupEditor;

namespace Ui {
class AddColumnWidget;
}

class AddColumnWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AddColumnWidget(K2QtTableModel *model, QWidget *parent = nullptr);
    ~AddColumnWidget();
private slots:
    void fillTypeChanged(int index);
    void dataTypeChanged(int index);
    void addColumn();
private:
    Ui::AddColumnWidget *m_ui;
    LineEdit *m_formula_field;
    QWidget *m_value_field;
    K2QtTableModel *m_model;
    KmtFuncPopupEditor *m_popup_editor;
};

#endif // ADDCOLUMNWIDGET_HPP
