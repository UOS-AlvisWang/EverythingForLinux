#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "define.h"

#include <DMainWindow>
#include <DLineEdit>
#include <DSpinner>
#include <dimagebutton.h>

#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QThread>
#include <QTableWidget>
#include <QPushButton>
#include <QSystemTrayIcon>

class Worker;
class TrayMenu;

DWIDGET_USE_NAMESPACE

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() Q_DECL_OVERRIDE;

    virtual void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;

public slots:
    void onSearchOver(QStringList lstFilePaths);
    void onBtnSearchClicked();
    void onCheckEnvOver(CheckEnvRst checkEnvRst);

signals:
    void sigSearch(QString fileName, SearchType searchType);
    void sigCheckEnv();

private:
    void initUi();
    void initConnection();
    void checkEnv();

    QString getSizeString(qint64 bitSize);
    void onMouseRightOnTableWgt();
    void onOpenFilePosition();
    void onSortTanleWgt(int index);

private:
    QVBoxLayout* vBoxLayoutMain = new QVBoxLayout();
    QHBoxLayout* hBoxLayoutHead = new QHBoxLayout();
    Worker* worker = nullptr;
    QThread* workThread = nullptr;
    DLineEdit* lineEidtFileName = nullptr;
    QPushButton* btnSearch = nullptr;
    QRadioButton* radioBtnExact = nullptr;
    QRadioButton* radioBtnFuzzy = nullptr;
    QTableWidget* tableWgtRst = nullptr;
    QMap<int, QString> rowWithFile;
    QMenu* tableWgtRstMenu = nullptr;
    QAction* actionOpen = nullptr;
    DSpinner* spinner = nullptr;
    TrayMenu* trayMenu = nullptr;
    QSystemTrayIcon* trayIcon = nullptr;
};

#endif // MAINWINDOW_H
