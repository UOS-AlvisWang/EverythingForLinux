#include "mainwindow.h"
#include <DApplication>

int main(int argc, char *argv[])
{
    qRegisterMetaType<SearchType>("SearchType");
    DApplication a(argc, argv);
    MainWindow w;
    w.setFixedSize(1200,800);
    w.setWindowIcon(QIcon(":/img/RSC/img/search.png"));
    w.show();

    return a.exec();
}
