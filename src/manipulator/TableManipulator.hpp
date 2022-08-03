#ifndef TABLEMANIPULATOR_HPP
#define TABLEMANIPULATOR_HPP

#include <QDockWidget>
#include "../K2QtTableModel.hpp"
#include "../DockWidget.hpp"

namespace Ui {
class TableManipulator;
}

class TableManipulator : public DockWidget
{
    Q_OBJECT
public:
    explicit TableManipulator(K2QtTableModel *model/*, const std::vector<QString> &default_values*/, QWidget *parent);
    ~TableManipulator();
private:
    Ui::TableManipulator *m_ui;
};

#endif // TABLEMANIPULATOR_HPP
