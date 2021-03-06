#include "mainwindow.h"
#include "Application.h"

#include <DApplication>
#include <QTextCodec>
#include <DWidgetUtil>

int main(int argc, char *argv[])
{
    qRegisterMetaType<SearchType>("SearchType");
    qRegisterMetaType<CheckEnvRst>("CheckEnvRst");
    Application::loadDXcbPlugin();  //让bar处在标题栏中
    Application *a = Application::getInstance(argc, argv);

    if (!a->setSingleInstance(a->applicationName()))
        exit(-1);

    a->setAttribute(Qt::AA_UseHighDpiPixmaps);
    a->loadTranslator();
    a->setOrganizationName("Alvis");
    a->setApplicationVersion(DApplication::buildVersion("1.0"));
    //a.setApplicationAcknowledgementPage("https://github.com/UOS-AlvisWang");
    a->setApplicationHomePage("https://github.com/UOS-AlvisWang");
    a->setProductIcon(QIcon(":/img/RSC/img/logo.ico"));
    a->setProductName("Everything For Linux");
    a->setApplicationName("Everything For Linux"); //只有在这儿修改窗口标题才有效

    MainWindow w;
    w.show();

    //让打开时界面显示在正中
    Dtk::Widget::moveToCenter(&w);


    return a->exec();
}
