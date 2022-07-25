#include "MainWindow.hpp"
#include <QApplication>
#include <QFile>
#include <QDebug>

#include <kmt/FunctionStore.hpp>
#include "IconProvider.hpp"



//QString toString(km::DataType dt)
//{
//    return K_SQ_STR(km::dataTypeToString(dt));
//}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle("Fusion");
    auto p = a.palette();
    p.setColor(QPalette::Highlight,QColor(29,111,66));
    a.setPalette(p);
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
