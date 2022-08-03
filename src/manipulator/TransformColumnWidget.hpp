#ifndef TRANSFORMCOLUMNWIDGET_HPP
#define TRANSFORMCOLUMNWIDGET_HPP

#include <QWidget>

#include <kmt/Core.hpp>
#include <kmt/Column.hpp>

class K2QtTableModel;

class KmtFuncPopupEditor;
namespace Ui {
class TransformColumnWidget;
}

class TransformColumnWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransformColumnWidget(K2QtTableModel *model, QWidget *parent = nullptr);

    ~TransformColumnWidget();
private slots:
    void transformColumn();
    void columnChanged(int index);
    void newColumnAdded(km::IndexType column_index, const km::ColumnMetaData &column_meta, int, const km::Variant& , const QString &);
private:
    Ui::TransformColumnWidget *m_ui;
    KmtFuncPopupEditor *m_popup_editor;
    K2QtTableModel * m_model;
};

#endif // TRANSFORMCOLUMNWIDGET_HPP
