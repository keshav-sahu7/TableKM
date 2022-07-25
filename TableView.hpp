#ifndef TABLEVIEW_HPP
#define TABLEVIEW_HPP

#include <QTableView>

#include "K2QtTableModel.hpp"

class TableView : public QTableView
{
    Q_OBJECT
public:
    TableView(QWidget *parent);
    void setModel(QAbstractItemModel *model) override;
    K2QtTableModel* getK2QtModel() const;
    virtual ~TableView();


private slots:
    void popupContextMenu(const QPoint &point);
    void formatChangeRequested();
    void alignmentChangeRequested();
    void sortViewModel();
    void foregroundColorChangeRequested();
    void backgroundColorChangeRequested();

private:
    QStringList getDisplayFormats(km::DataType data_type);

private:
    static QList<std::pair<QString, QColor>> colors;

private:
    K2QtTableModel *m_model;


};

inline K2QtTableModel *TableView::getK2QtModel() const
{
    return m_model;
}

#endif // TABLEVIEW_HPP
