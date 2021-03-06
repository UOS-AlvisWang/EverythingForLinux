#include "worker.h"

#include <QFileInfo>
#include <QDebug>
#include <DInputDialog>

Worker::Worker(QObject *parent) : QObject(parent)
  , process(new QProcess(this))
  , processCheckEnv(new QProcess(this))
  , processUpdateDb(new QProcess(this))
{
    connect(process, &QProcess::readyReadStandardOutput, this, &Worker::onRead);
    connect(process, &QProcess::stateChanged, this, &Worker::onProcessStateChanged);
    connect(processCheckEnv, &QProcess::readyReadStandardOutput, this, &Worker::onReadCheckEnv);
    connect(processUpdateDb, &QProcess::readyReadStandardOutput, this, &Worker::onReadUpdateDb);
}

void Worker::onSearch(QString fileName, SearchType searchType)
{
    if(fileName.isEmpty())
    {
        return;
    }

    QString shell = "";

    if(searchType == SearchType::Exact)
    {
        shell = QString("find / -name %1").arg(fileName);
    }

    if(searchType == SearchType::Fuzzy)
    {
        shell = QString("locate -l 1000 %1").arg(fileName);
    }

    process->start(shell);
}

void Worker::onRead()
{
    QByteArray bytes = process->readAllStandardOutput();
    QStringList files = QString(bytes).split("\n");
    lstFilePaths.append(files);
}

void Worker::onProcessStateChanged(QProcess::ProcessState state)
{
    QStringList lstTmp;
    switch (state) {
    case QProcess::NotRunning:

        for(int i = 0; i < lstFilePaths.size(); i++)
        {
            QFileInfo fileInfo(lstFilePaths[i]);

            if(fileInfo.exists())
            {
                lstTmp.append(lstFilePaths[i]);
            }
        }

        emit sigSearchOver(lstTmp);
        break;
    case QProcess::Starting:
        lstFilePaths.clear();
        break;
    default:
        break;
    }
}

void Worker::onCheckEnv()
{
    QString shell = "dpkg -s locate";
    processCheckEnv->start(shell);
}

void Worker::onReadCheckEnv()
{
    QByteArray bytes = processCheckEnv->readAllStandardOutput();
    QString output = QString(bytes);

    if(output.indexOf("deinstall") != -1)
    {
        emit sigCheckEnvRst(NoLocate);
        return;
    }

    emit sigCheckEnvRst(Normal);
}

void Worker::onUpdateDb(QString passwd)
{
    QString shell = QString("echo %1 | sudo -S %2").arg(passwd).arg("updatedb");
    processUpdateDb->start(shell);
    qDebug() << "start" << shell;
}

void Worker::onReadUpdateDb()
{
    qDebug() << "read";
    QByteArray bytes = processCheckEnv->readAllStandardOutput();
    QString output = QString(bytes);
    qDebug() << output;
}
