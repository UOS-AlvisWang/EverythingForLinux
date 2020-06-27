#include "mainwindow.h"
#include "worker.h"

#include <DTitlebar>
#include <DSpinner>

#include <QMessageBox>
#include <QInputDialog>
#include <QFrame>
#include <QHeaderView>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QDateTime>
#include <QAction>
#include <QDesktopServices>
#include <QUrl>
#include <QTextCodec>
#include <QKeySequence>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) :
    DMainWindow(parent)
  , worker(new Worker())
  , workThread(new QThread())
  , lineEidtFileName(new DLineEdit(this))
  , btnSearch(new QPushButton(this))
  , radioBtnExact(new QRadioButton(this))
  , radioBtnFuzzy(new QRadioButton(this))
  , tableWgtRst(new QTableWidget(this))
  , tableWgtRstMenu(new QMenu())
  , actionOpen(new QAction("打开文件所在位置", tableWgtRstMenu))
  , spinner(new DSpinner(this))
{
    worker->moveToThread(workThread);
    workThread->start();
    initUi();
    initConnection();
    checkEnv();
}

MainWindow::~MainWindow()
{
     workThread->quit();
     workThread->wait();

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
    setFixedSize(1200, 800);
    spinner->move(this->width()/2 - spinner->width()/2, this->height()/2 - spinner->height()/2);
    spinner->setFixedSize(100,100);
    spinner->start();
    spinner->hide();
    lineEidtFileName->setPlaceholderText("输入要查找的文件名");
    btnSearch->setShortcut(Qt::Key_Enter);
    btnSearch->setShortcut(Qt::Key_Return);
    btnSearch->setIcon(QIcon("qrc:/img/RSC/img/search.png"));
    btnSearch->setText("搜索");
    radioBtnExact->setText("精确查找");
    radioBtnExact->setChecked(true);
    radioBtnFuzzy->setText("模糊查找");

    tableWgtRst->setColumnCount(5);
    tableWgtRst->horizontalHeader()->resizeSection(0, 100);
    tableWgtRst->horizontalHeader()->resizeSection(1, 150);
    tableWgtRst->horizontalHeader()->resizeSection(2, 500);
    tableWgtRst->horizontalHeader()->resizeSection(3, 100);
    tableWgtRst->horizontalHeader()->resizeSection(4, 300);
    tableWgtRst->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWgtRst->horizontalHeader()->setStretchLastSection(true);
    tableWgtRst->verticalHeader()->setVisible(false);
    tableWgtRst->setSelectionMode(QAbstractItemView::SingleSelection);

    QFile file(":/RSC/img/tableWidget.qss");

    if(file.open(QFile::ReadOnly))
    {
      QString strStyleSheet = file.readAll();
      file.close();
      tableWgtRst->setStyleSheet(strStyleSheet);
    }

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

    tableWgtRstMenu->addAction(actionOpen);
    tableWgtRst->setContextMenuPolicy(Qt::CustomContextMenu);
}

void MainWindow::initConnection()
{
    connect(this, &MainWindow::sigCheckEnv, worker, &Worker::onCheckEnv, Qt::QueuedConnection);
    connect(worker, &Worker::sigCheckEnvRst, this, &MainWindow::onCheckEnvOver, Qt::QueuedConnection);
    connect(this, &MainWindow::sigSearch, worker, &Worker::onSearch, Qt::QueuedConnection);
    connect(worker, &Worker::sigSearchOver, this, &MainWindow::onSearchOver, Qt::QueuedConnection);
    connect(btnSearch, &QPushButton::clicked, this, &MainWindow::onBtnSearchClicked, Qt::QueuedConnection);
    connect(tableWgtRst, &QTableWidget::customContextMenuRequested, this, &MainWindow::onMouseRightOnTableWgt);
    connect(actionOpen, &QAction::triggered, this, &MainWindow::onOpenFilePosition);
}

void MainWindow::checkEnv()
{
    emit sigCheckEnv();
}

void MainWindow::onSearchOver(QStringList lstFilePaths)
{
    spinner->hide();
    rowWithFile.clear();
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

    tableWgtRst->clearContents();
    tableWgtRst->setRowCount(0);
    SearchType searchType = SearchType(radioBtnExact->isChecked() ? 0 : 1);
    emit sigSearch(fileName, searchType);
    spinner->show();
}

void MainWindow::onCheckEnvOver(CheckEnvRst checkEnvRst)
{
    if(checkEnvRst == NoLocate)
    {
        QMessageBox::warning(this, "安装locate", "检测到您的系统未安装locate，不能正常使用模糊查找的功能,请先安装locate。");
        radioBtnFuzzy->setEnabled(false);
    }
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

void MainWindow::onMouseRightOnTableWgt()
{
    QTableWidgetItem* item = tableWgtRst->currentItem();

    if(!item)
    {
        return;
    }

    tableWgtRstMenu->exec(QCursor::pos());
}

void MainWindow::onOpenFilePosition()
{
    QTableWidgetItem* item = tableWgtRst->currentItem();

    if(!item)
    {
        return;
    }

    int row = item->row();
    QString strFilePath = rowWithFile[row];
    QDesktopServices::openUrl(QUrl(strFilePath.left(strFilePath.lastIndexOf("/"))));
}
