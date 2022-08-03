#include "DockWidget.hpp"

#include <QCloseEvent>



void DockWidget::addVisibilityActionInMenu(QMenu *menu, QWidget *parent, const QString &title)
{
    QAction *action = new QAction( title.isEmpty() ? windowTitle() : title, parent);
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, &QAction::toggled, this, &DockWidget::setVisible);
    connect(this, &DockWidget::closed, [action]{
        action->setChecked(false);
    });
    connect(this, &DockWidget::destroyed, action, &QAction::deleteLater);
    menu->addAction(action);
}

void DockWidget::closeEvent(QCloseEvent *event)
{
    QDockWidget::closeEvent(event);
    emit closed();
}
