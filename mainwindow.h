#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "define.h"

#include <DMainWindow>
#include <DLineEdit>
#include <dpushbutton.h>

#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QThread>
#include <QTableWidget>

class Worker;

DWIDGET_USE_NAMESPACE

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initUi();
    void initConnection();

public slots:
    void onSearchOver(QStringList lstFilePaths);
    void onBtnSearchClicked();

signals:
    void sigSearch(QString fileName, SearchType searchType);

private:
    QString getSizeString(qint64 bitSize);
    void onMouseRightOnTableWgt();
    void onOpenFilePosition();

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
    QMap<int, QString> rowWithFile;
    QMenu* tableWgtRstMenu = nullptr;
    QAction* actionOpen = nullptr;
};

#endif // MAINWINDOW_H
