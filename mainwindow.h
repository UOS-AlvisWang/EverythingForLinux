#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "define.h"

#include <DMainWindow>
#include <DSearchEdit>
#include <DSpinner>
#include <dimagebutton.h>
#include <DToolButton>

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
    void onFilePathEditingFinished();
    void onCheckEnvOver(CheckEnvRst checkEnvRst);

signals:
    void sigSearch(QString fileName, SearchType searchType);
    void sigCheckEnv();
    void sigUpdateDb(QString passwd);

private:
    void initUi();
    void initConnection();
    void checkEnv();

    QString getSizeString(qint64 bitSize);
    void onMouseRightOnTableWgt();
    void onOpenFilePosition();
    void onCopyPath();
    void onCopy();
    void onSortTanleWgt(int index);
    void onUpdateDb();

private:
    QVBoxLayout* vBoxLayoutMain = new QVBoxLayout();
    QHBoxLayout* hBoxLayoutHead = new QHBoxLayout();
    QVBoxLayout* vBoxRadioBtns = new QVBoxLayout();
    Worker* worker = nullptr;
    QThread* workThread = nullptr;
    DSearchEdit* searchEidtFileName = nullptr;
    QRadioButton* radioBtnExact = nullptr;
    QRadioButton* radioBtnFuzzy = nullptr;
    QTableWidget* tableWgtRst = nullptr;
    QMenu* tableWgtRstMenu = nullptr;
    QAction* actionOpen = nullptr;
    QAction* actionCopyPath = nullptr;
    QAction* actionCopy = nullptr;
    DSpinner* spinner = nullptr;
    TrayMenu* trayMenu = nullptr;
    QSystemTrayIcon* trayIcon = nullptr;
};

#endif // MAINWINDOW_H
