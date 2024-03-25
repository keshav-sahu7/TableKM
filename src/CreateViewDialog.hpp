#ifndef CREATEVIEWDIALOG_HPP
#define CREATEVIEWDIALOG_HPP

#include <QDialog>
#include <kmt/AbstractTable.hpp>
#include <kmt/BasicView.hpp>

namespace Ui {
class ViewDialog;
}

class KmtFuncPopupEditor;

/**
 * This dialog *creates* a view of existing table.
 */
class CreateViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateViewDialog(km::AbstractTable *source, QWidget *parent = nullptr);
    km::BasicView *getView();
    ~CreateViewDialog();
private slots:
    void columnMoveUp();
    void columnMoveDown();
    void selectAll();
    void deselectAll();
    void invertSelections();
    void createView();
    void popupEditor();
    void popupEditorLostFocus();
private:
    void swapRows(int i1, int i2);
private:
    Ui::ViewDialog *m_ui;
    km::BasicView *m_view;
    km::AbstractTable *m_source;
    KmtFuncPopupEditor *m_popup_editor;
};


#endif // CREATEVIEWDIALOG_HPP
