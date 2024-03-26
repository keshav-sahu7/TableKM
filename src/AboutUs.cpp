#include "AboutUs.hpp"
#include "ui_AboutUs.h"
#include "config.h"
#include "ThemeHandler2.hpp"

AboutUs::AboutUs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutUs)
{
    ui->setupUi(this);
    const auto color = Theme::getInstance().getFlavourColor();
    const auto colorString = QString("rgb(%1, %2, %3)")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue());
    const auto title_html = QString("<html><head/><body><p><span style=\"font-size:22pt; font-weight:700; font-style:italic; color:%1;\">TableKM</span><span style=\" font-size:22pt; font-weight:700; font-style:italic; color:%1; vertical-align:super;\">v%2.%3</span></p></body></html>")
                                .arg(colorString)
                                .arg(PROJECT_VERSION_MAJOR)
                                .arg(PROJECT_VERSION_MINOR);
    ui->lb_title->setText(title_html);
}

AboutUs::~AboutUs()
{
    delete ui;
}
