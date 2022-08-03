#ifndef AUTOVALUEGENERATORSETTINGSWINDOW_HPP
#define AUTOVALUEGENERATORSETTINGSWINDOW_HPP

#include <QWidget>
#include <QTableWidget>

#include "AddRowWidget.hpp"
#include "DataEvaluator.hpp"

class KmtFuncPopupEditor;

namespace Ui
{
class AutoValueGeneratorSettingsWindow;
}

class AutoValueGeneratorSettingsWindow : public QDialog
{
    Q_OBJECT
public:
    AutoValueGeneratorSettingsWindow(DataEvaluator *evaluator,
                                     AddRowWidget *parent,
                                     bool as_dialog = true);
    QList<DataEvaluator::EvalData_> getEvaluatorData() const;
private slots:
    void fillTypeChanged(int index);
    void showFormulaEditor();
    void popupEditorHidden();
    void applySettings();
    void statusChanged();
private:
    void addColumn(int index);
private:
    Ui::AutoValueGeneratorSettingsWindow *m_ui;
    const km::Table *m_table;
    KmtFuncPopupEditor *m_popup_editor;
    QList<DataEvaluator::EvalData_> m_eval_data;
    DataEvaluator *m_evaluator;
    QList<bool> m_status_list;

};

#endif // AUTOVALUEGENERATORSETTINGSWINDOW_HPP
