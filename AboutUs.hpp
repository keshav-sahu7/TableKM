#ifndef ABOUTUS_HPP
#define ABOUTUS_HPP

#include <QDialog>

namespace Ui {
class AboutUs;
}

class AboutUs : public QDialog
{
    Q_OBJECT

public:
    explicit AboutUs(QWidget *parent = nullptr);
    ~AboutUs();

private:
    Ui::AboutUs *ui;
};

#endif // ABOUTUS_HPP
