#include "mainwindow.h"
#include "worker.h"

#include <QFrame>
#include <QHeaderView>

MainWindow::MainWindow(DMainWindow *parent) :
    DMainWindow(parent)
  , worker(new Worker())
  , workThread(new QThread(this))
  , lineEidtFileName(new DLineEdit(this))
  , btnSearch(new DPushButton(this))
  , radioBtnExact(new QRadioButton(this))
  , radioBtnFuzzy(new QRadioButton(this))
  , tableWgtRst(new QTableWidget(this))
{
    worker->moveToThread(workThread);
    initUi();
    initConnection();
}

MainWindow::~MainWindow()
{
    if(workThread->isRunning())
    {
        workThread->wait();
        workThread->quit();
    }

    if(vBoxLayoutMain)
    {
        delete vBoxLayoutMain;
        vBoxLayoutMain = nullptr;
    }

    if(hBoxLayoutHead)
    {
        delete hBoxLayoutHead;
        hBoxLayoutHead = nullptr;
    }

    if(worker)
    {
        delete worker;
        worker = nullptr;
    }
}

void MainWindow::initUi()
{

    lineEidtFileName->setPlaceholderText("输入要查找的文件名");
    btnSearch->setText("搜索");
    //btnSearch->setIcon(QIcon("qrc:/img/RSC/img/search.png"));
    radioBtnExact->setText("精确查找");
    radioBtnExact->setChecked(true);
    radioBtnFuzzy->setText("模糊查找");

    tableWgtRst->setColumnCount(5);
    tableWgtRst->horizontalHeader()->resizeSection(0, 100);
    tableWgtRst->horizontalHeader()->resizeSection(1, 150);
    tableWgtRst->horizontalHeader()->resizeSection(2, 500);
    tableWgtRst->horizontalHeader()->resizeSection(3, 100);
    tableWgtRst->horizontalHeader()->resizeSection(4, 400);
    tableWgtRst->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWgtRst->horizontalHeader()->setStretchLastSection(true);
    tableWgtRst->verticalHeader()->setVisible(false);

    QStringList lstHeader;
    lstHeader << "序号" <<"文件名" << "文件路径" << "文件大小" << "修改时间";
    tableWgtRst->setHorizontalHeaderLabels(lstHeader);

    hBoxLayoutHead->addStretch(2);
    hBoxLayoutHead->addWidget(lineEidtFileName, 5);
    hBoxLayoutHead->addWidget(radioBtnExact, 1);
    hBoxLayoutHead->addWidget(radioBtnFuzzy, 1);
    hBoxLayoutHead->addWidget(btnSearch, 1);
    hBoxLayoutHead->addStretch(2);

    vBoxLayoutMain->addLayout(hBoxLayoutHead);
    vBoxLayoutMain->addSpacing(30);
    vBoxLayoutMain->addWidget(tableWgtRst);

    QFrame* frame = new QFrame(this);
    setCentralWidget(frame);
    frame->setLayout(vBoxLayoutMain);
}

void MainWindow::initConnection()
{

}
