#ifndef DOCKWIDGET_HPP
#define DOCKWIDGET_HPP

#include <QDockWidget>
#include <QMenu>

class DockWidget : public QDockWidget
{
    Q_OBJECT
public:
    DockWidget(const QString &title, QWidget *parent);
    void addVisibilityActionInMenu(QMenu *menu, QWidget *parent, const QString &title = QString());
signals:
    void closed();
protected:
    virtual void closeEvent(QCloseEvent *event) override;
};

inline DockWidget::DockWidget(const QString &title, QWidget *parent) : QDockWidget(title, parent)
{

}

#endif // DOCKWIDGET_HPP
