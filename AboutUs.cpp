#include "AboutUs.hpp"
#include "ui_AboutUs.h"

AboutUs::AboutUs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutUs)
{
    ui->setupUi(this);
}

AboutUs::~AboutUs()
{
    delete ui;
}
