#include "mainwindow.h"
#include <DApplication>

int main(int argc, char *argv[])
{
    DApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
