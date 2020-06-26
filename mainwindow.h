#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <dmainwindow.h>
#include <DLineEdit>
#include <dpushbutton.h>

#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QThread>
#include <QTableWidget>

class Worker;

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

private:
    QVBoxLayout* vBoxLayoutMain = new QVBoxLayout();
    QHBoxLayout* hBoxLayoutHead = new QHBoxLayout();
    Worker* worker = nullptr;
    QThread* workThread = nullptr;
    DLineEdit* lineEidtFileName = nullptr;
    DPushButton* btnSearch = nullptr;
    QRadioButton* radioBtnExact = nullptr;
    QRadioButton* radioBtnFuzzy = nullptr;
    QTableWidget* tableWgtRst = nullptr;
};

#endif // MAINWINDOW_H
