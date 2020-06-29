#ifndef TRAYMENU_H
#define TRAYMENU_H

#include <QObject>
#include <QAction>
#include <QMenu>

class TrayMenu : public QMenu
{
    Q_OBJECT

public:
    TrayMenu(QWidget* parent = nullptr);
    Q_DISABLE_COPY(TrayMenu)
    virtual ~TrayMenu() Q_DECL_OVERRIDE;

signals:
    void sigQuit();

private:
    void onQuitAction();

private:
    QAction* actionQuit = nullptr;
};

#endif // TRAYMENU_H
