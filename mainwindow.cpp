#include "mainwindow.h"
#include "worker.h"

#include <DTitlebar>

#include <QFrame>
#include <QHeaderView>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QDateTime>

MainWindow::MainWindow(DMainWindow *parent) :
    DMainWindow(parent)
  , worker(new Worker())
  , workThread(new QThread())
  , lineEidtFileName(new DLineEdit(this))
  , btnSearch(new DPushButton(this))
  , radioBtnExact(new QRadioButton(this))
  , radioBtnFuzzy(new QRadioButton(this))
  , tableWgtRst(new QTableWidget(this))
{
    worker->moveToThread(workThread);
    workThread->start();
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
        vBoxLayoutMain->deleteLater();;
        vBoxLayoutMain = nullptr;
    }

    if(hBoxLayoutHead)
    {
        hBoxLayoutHead->deleteLater();
        hBoxLayoutHead = nullptr;
    }

    if(worker)
    {
        worker->deleteLater();
        worker = nullptr;
    }

    if(workThread)
    {
        workThread->deleteLater();
    }
}

void MainWindow::initUi()
{
    titlebar()->setVisible(false);
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
    connect(this, &MainWindow::sigSearch, worker, &Worker::onSearch, Qt::QueuedConnection);
    connect(worker, &Worker::sigSearchOver, this, &MainWindow::onSearchOver, Qt::QueuedConnection);
    connect(btnSearch, &DPushButton::clicked, this, &MainWindow::onBtnSearchClicked, Qt::QueuedConnection);
}

void MainWindow::onSearchOver(QStringList lstFilePaths)
{
    tableWgtRst->clearContents();
    tableWgtRst->setRowCount(lstFilePaths.count());

    int rowIndex = 0;

    for(QString filePath : lstFilePaths)
    {
        if(filePath.isEmpty())
        {
            continue;
        }

        QFileInfo fileInfo(filePath);

        if(!fileInfo.exists())
        {
            continue;
        }

        QString fileName = fileInfo.fileName();
        QString fileSize = getSizeString(fileInfo.size());
        QFileIconProvider iconProvider;
        QIcon fileIcon = iconProvider.icon(fileInfo);

        if(fileIcon.isNull())
        {
            fileIcon = QIcon(":/img/RSC/img/unknowFile.png");
        }

        QString changeTime = fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss");

        QTableWidgetItem* indexItem = new QTableWidgetItem(QString::number(rowIndex+1));
        indexItem->setTextAlignment(Qt::AlignCenter);
        tableWgtRst->setItem(rowIndex, 0, indexItem);
        tableWgtRst->setItem(rowIndex, 1, new QTableWidgetItem(fileIcon, fileName));
        tableWgtRst->setItem(rowIndex, 2, new QTableWidgetItem(filePath));
        tableWgtRst->setItem(rowIndex, 3, new QTableWidgetItem(fileSize));
        QTableWidgetItem* changeTimeItem = new QTableWidgetItem(changeTime);
        changeTimeItem->setTextAlignment(Qt::AlignCenter);
        tableWgtRst->setItem(rowIndex, 4, changeTimeItem);

        rowWithFile.insert(rowIndex, fileInfo.filePath());
        rowIndex++;
    }
}

void MainWindow::onBtnSearchClicked()
{
    QString fileName = lineEidtFileName->text();

    if(fileName.isEmpty())
    {
        return;
    }

    SearchType searchType = SearchType(radioBtnExact->isChecked() ? 0 : 1);
    emit sigSearch(fileName, searchType);
}

QString MainWindow::getSizeString(qint64 bitSize)
{
    QString size = "";

    if(bitSize < 1024)
    {
        size = QString::number(bitSize) + "B";
    }

    if((bitSize > 1024) && (bitSize < 1024 * 1024))
    {
        size = QString::number(bitSize/1024.0, 'f', 2) + "KB";
    }

    if((bitSize > 1024 * 1024) && (bitSize < 1024 * 1024 * 1024))
    {
        size = QString::number(bitSize/1024/1024.0, 'f', 2) + "MB";
    }

    if(bitSize > 1024 * 1024 * 1024)
    {
        size = QString::number(bitSize/1024/1024/1024.0, 'f', 2) + "G";
    }

    return size;
}
