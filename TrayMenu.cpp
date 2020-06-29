#include "TrayMenu.h"

TrayMenu::TrayMenu(QWidget* parent) :
    QMenu(parent)
  , actionQuit(new QAction(this))
{
    setIcon(QIcon(":/img/RSC/img/search.png"));
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

