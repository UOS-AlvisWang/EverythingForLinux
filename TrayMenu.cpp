#include "TrayMenu.h"

TrayMenu::TrayMenu(QWidget* parent) :
    QMenu(parent)
  , actionQuit(new QAction(this))
  , actionUpdateDb(new QAction(this))
{
    setIcon(QIcon(":/img/RSC/img/search.png"));

    actionUpdateDb->setIcon(QIcon(":/img/RSC/img/update.png"));
    actionUpdateDb->setText("更新Locate数据库");
    connect(actionUpdateDb, &QAction::triggered, this, &TrayMenu::onUpdateDbAction);
    //addAction(actionUpdateDb);

    actionQuit->setIcon(QIcon(":/img/RSC/img/quit.png"));
    actionQuit->setText("退出");
    connect(actionQuit, &QAction::triggered, this, &TrayMenu::onQuitAction);
    addAction(actionQuit);
}

TrayMenu::~TrayMenu()
{

}

void TrayMenu::onQuitAction()
{
    emit sigQuit();
}

void TrayMenu::onUpdateDbAction()
{
    emit sigUpdateDb();
}

