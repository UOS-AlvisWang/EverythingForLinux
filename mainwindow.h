#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <dmainwindow.h>

namespace Ui {
class MainWindow;
}

using namespace Dtk::Widget;

using namespace Dtk::Widget;

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(DMainWindow *parent = 0);
    ~MainWindow();

    void initUi();
    void initConnection();
};

#endif // MAINWINDOW_H
