#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QFile>

#include <kmt/FunctionStore.hpp>
#include "ThemeHandler2.hpp"
#include "MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Theme::setTheme("dark","green");
    try{
        km::initAllFnc();
        MainWindow w;
        w.setWindowIcon(icons::getIcon("table"));
        w.show();
        return a.exec();
    }
    catch(std::exception &e)
    {
        qDebug() << e.what() << Qt::endl;
        return -1;
    }
}
