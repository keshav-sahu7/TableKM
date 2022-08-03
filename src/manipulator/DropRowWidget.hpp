#ifndef DROPROWWIDGET_HPP
#define DROPROWWIDGET_HPP

#include <QDialog>


class K2QtTableModel;
namespace Ui {
class DropRowWidget;
}

class DropRowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DropRowWidget(K2QtTableModel *model, QWidget *parent = nullptr);
    ~DropRowWidget();
private slots:
    void dropRow();
private:
    Ui::DropRowWidget *m_ui;
    K2QtTableModel *m_model;
};

#endif // DROPROWWIDGET_HPP
