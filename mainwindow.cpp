#include "TrayMenu.h"
#include "mainwindow.h"
#include "worker.h"
#include "Application.h"

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
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QClipboard>
#include <QMimeData>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    DMainWindow(parent)
    , worker(new Worker())
    , workThread(new QThread())
    , searchEidtFileName(new DSearchEdit(this))
    , radioBtnExact(new QRadioButton(this))
    , radioBtnFuzzy(new QRadioButton(this))
    , tableWgtRst(new QTableWidget(this))
    , tableWgtRstMenu(new QMenu())
    , actionOpen(new QAction("打开文件所在位置", tableWgtRstMenu))
    , actionCopyPath(new QAction("复制文件路径", tableWgtRstMenu))
    , actionCopy(new QAction("复制", tableWgtRstMenu))
    , spinner(new DSpinner(this))
    , trayMenu(new TrayMenu(this))
    , trayIcon(new QSystemTrayIcon(this))
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

    if (vBoxLayoutMain) {
        vBoxLayoutMain->deleteLater();;
        vBoxLayoutMain = nullptr;
    }

    if (hBoxLayoutHead) {
        hBoxLayoutHead->deleteLater();
        hBoxLayoutHead = nullptr;
    }

    if (vBoxRadioBtns) {
        vBoxRadioBtns->deleteLater();
        vBoxRadioBtns = nullptr;
    }

    if (worker) {
        worker->deleteLater();
        worker = nullptr;
    }

    if (workThread) {
        workThread->deleteLater();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    setVisible(!isVisible());
    event->ignore();
}

void MainWindow::initUi()
{
    setFixedSize(1200, 800);
    titlebar()->setIcon(QIcon(":/img/RSC/img/search.png"));
    titlebar()->setMenuVisible(false);
    spinner->move(this->width() / 2 - spinner->width() / 2, this->height() / 2 - spinner->height() / 2);
    spinner->setFixedSize(100, 100);
    spinner->start();
    spinner->hide();
    searchEidtFileName->setPlaceholderText("搜索文件");
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
    tableWgtRst->horizontalHeader()->setSortIndicatorShown(true);       //显示排序图标（默认为上下箭头）

    QFile file(":/RSC/img/tableWidget.qss");

    if (file.open(QFile::ReadOnly)) {
        QString strStyleSheet = file.readAll();
        file.close();
        tableWgtRst->setStyleSheet(strStyleSheet);
    }

    QStringList lstHeader;
    lstHeader << "序号" << "文件名" << "文件路径" << "文件大小" << "修改时间";
    tableWgtRst->setHorizontalHeaderLabels(lstHeader);

    hBoxLayoutHead->addStretch(2);
    hBoxLayoutHead->addWidget(searchEidtFileName, 5);
    hBoxLayoutHead->addSpacing(20);
    vBoxRadioBtns->addWidget(radioBtnExact, 1);
    vBoxRadioBtns->addWidget(radioBtnFuzzy, 1);
    hBoxLayoutHead->addLayout(vBoxRadioBtns, 1);
    hBoxLayoutHead->addStretch(2);

    vBoxLayoutMain->addLayout(hBoxLayoutHead);
    vBoxLayoutMain->addSpacing(30);
    vBoxLayoutMain->addWidget(tableWgtRst);

    QFrame *frame = new QFrame(this);
    setCentralWidget(frame);
    frame->setLayout(vBoxLayoutMain);

    tableWgtRstMenu->addAction(actionCopy);
    tableWgtRstMenu->addAction(actionCopyPath);
    tableWgtRstMenu->addAction(actionOpen);
    tableWgtRst->setContextMenuPolicy(Qt::CustomContextMenu);

    trayIcon ->setToolTip(QString("搜索文件"));
    trayIcon ->setIcon(QIcon(":/img/RSC/img/search.png"));
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
}

void MainWindow::initConnection()
{
    connect(this, &MainWindow::sigCheckEnv, worker, &Worker::onCheckEnv, Qt::QueuedConnection);
    connect(worker, &Worker::sigCheckEnvRst, this, &MainWindow::onCheckEnvOver, Qt::QueuedConnection);
    connect(this, &MainWindow::sigSearch, worker, &Worker::onSearch, Qt::QueuedConnection);
    connect(worker, &Worker::sigSearchOver, this, &MainWindow::onSearchOver, Qt::QueuedConnection);
    connect(searchEidtFileName, &DSearchEdit::returnPressed, this, &MainWindow::onFilePathEditingFinished, Qt::QueuedConnection);
    connect(tableWgtRst, &QTableWidget::customContextMenuRequested, this, &MainWindow::onMouseRightOnTableWgt);
    connect(actionOpen, &QAction::triggered, this, &MainWindow::onOpenFilePosition);
    connect(actionCopyPath, &QAction::triggered, this, &MainWindow::onCopyPath);
    connect(actionCopy, &QAction::triggered, this, &MainWindow::onCopy);
    connect(tableWgtRst->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::onSortTanleWgt);

    connect(trayMenu, &TrayMenu::sigQuit, Application::instance(), &Application::quit);
    connect(trayIcon, &QSystemTrayIcon::activated, this, [ = ](QSystemTrayIcon::ActivationReason reason) {
        Q_UNUSED(reason)
        this->setVisible(!isVisible());
    });

    connect(radioBtnFuzzy, &QRadioButton::clicked, this, [ = ](bool checked) {
        if (!checked)
            return ;
    });

    connect(trayMenu, &TrayMenu::sigUpdateDb, this, &MainWindow::onUpdateDb);
    connect(this, &MainWindow::sigUpdateDb, worker, &Worker::onUpdateDb);
}

void MainWindow::checkEnv()
{
    emit sigCheckEnv();
}

void MainWindow::onSearchOver(QStringList lstFilePaths)
{
    spinner->hide();
    tableWgtRst->clearContents();
    tableWgtRst->setRowCount(lstFilePaths.count());

    int rowIndex = 0;

    for (QString filePath : lstFilePaths) {
        if (filePath.isEmpty()) {
            continue;
        }

        QFileInfo fileInfo(filePath);

        if (!fileInfo.exists()) {
            continue;
        }

        QString fileName = fileInfo.fileName();
        QString fileSize = getSizeString(fileInfo.size());
        QFileIconProvider iconProvider;
        QIcon fileIcon = iconProvider.icon(fileInfo);

        if (fileIcon.isNull()) {
            fileIcon = QIcon(":/img/RSC/img/unknowFile.png");
        }

        QString changeTime = fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss");
        QTableWidgetItem *indexItem = new QTableWidgetItem(QString::number(rowIndex + 1));
        indexItem->setTextAlignment(Qt::AlignCenter);
        tableWgtRst->setItem(rowIndex, 0, indexItem);
        tableWgtRst->setItem(rowIndex, 1, new QTableWidgetItem(fileIcon, fileName));
        tableWgtRst->setItem(rowIndex, 2, new QTableWidgetItem(filePath));
        tableWgtRst->setItem(rowIndex, 3, new QTableWidgetItem(fileSize));
        QTableWidgetItem *changeTimeItem = new QTableWidgetItem(changeTime);
        changeTimeItem->setTextAlignment(Qt::AlignCenter);
        tableWgtRst->setItem(rowIndex, 4, changeTimeItem);
        rowIndex++;
    }

    tableWgtRst->sortByColumn(4);
}

void MainWindow::onFilePathEditingFinished()
{
    QString fileName = searchEidtFileName->text();

    if (fileName.isEmpty()) {
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
    if (checkEnvRst == NoLocate) {
        QMessageBox::warning(this, "安装locate", "检测到您的系统未安装locate，不能正常使用模糊查找的功能,请先安装locate。");
        radioBtnFuzzy->setEnabled(false);
    }
}

QString MainWindow::getSizeString(qint64 bitSize)
{
    QString size = "";

    if (bitSize < 1024) {
        size = QString::number(bitSize) + "B";
    }

    if ((bitSize > 1024) && (bitSize < 1024 * 1024)) {
        size = QString::number(bitSize / 1024.0, 'f', 2) + "KB";
    }

    if ((bitSize > 1024 * 1024) && (bitSize < 1024 * 1024 * 1024)) {
        size = QString::number(bitSize / 1024 / 1024.0, 'f', 2) + "MB";
    }

    if (bitSize > 1024 * 1024 * 1024) {
        size = QString::number(bitSize / 1024 / 1024 / 1024.0, 'f', 2) + "G";
    }

    return size;
}

void MainWindow::onMouseRightOnTableWgt()
{
    QTableWidgetItem *item = tableWgtRst->currentItem();

    if (!item) {
        return;
    }

    tableWgtRstMenu->exec(QCursor::pos());
}

void MainWindow::onOpenFilePosition()
{
    int row = tableWgtRst->currentRow();
    QString strFilePath = tableWgtRst->item(row, 2)->text();
    QDesktopServices::openUrl(QUrl::fromLocalFile(strFilePath.left(strFilePath.lastIndexOf("/"))));
}

void MainWindow::onCopyPath()
{
    int row = tableWgtRst->currentRow();
    QString strFilePath = tableWgtRst->item(row, 2)->text();
    QClipboard *clip = QApplication::clipboard();

    clip->setText(strFilePath);
}

void MainWindow::onCopy()
{
    int row = tableWgtRst->currentRow();
    QString strFilePath = tableWgtRst->item(row, 2)->text();
    QClipboard *cb = QApplication::clipboard();
    QMimeData *newMimeData = new QMimeData();
    newMimeData->setData("x-special/gnome-copied-files", QByteArray(QString("copy\nfile://%1").arg(strFilePath).toUtf8()));
    newMimeData->setData("text/uri-list", QByteArray(QString("file://%1").arg(strFilePath).toUtf8()));
    cb->setMimeData(newMimeData);
}

void MainWindow::onSortTanleWgt(int index)
{
    tableWgtRst->sortByColumn(index);
}

void MainWindow::onUpdateDb()
{
    QString passwd = DInputDialog::getText(nullptr, "sudo密码", "需要sudo权限");
    if (!passwd.isEmpty()) {
        emit sigUpdateDb(passwd);
    }
}
