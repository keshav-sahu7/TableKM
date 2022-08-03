#ifndef ABOUTUS_HPP
#define ABOUTUS_HPP

#include <QDialog>

namespace Ui {
class AboutUs;
}

/**
 * AboutUs class
 */
class AboutUs : public QDialog
{
    Q_OBJECT

    Q_DISABLE_COPY_MOVE(AboutUs)

public:
    explicit AboutUs(QWidget *parent = nullptr);
    ~AboutUs();

private:
    Ui::AboutUs *ui;
};

#endif // ABOUTUS_HPP
