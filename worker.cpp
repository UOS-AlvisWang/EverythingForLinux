#include "worker.h"

Worker::Worker(QObject *parent) : QObject(parent)
  , process(new QProcess(this))
{
    connect(process, &QProcess::readyReadStandardOutput, this, &Worker::onRead);
    connect(process, &QProcess::stateChanged, this, &Worker::onProcessStateChanged);
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
        shell = QString("locate %1").arg(fileName);
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
    switch (state) {
    case QProcess::NotRunning:

        while(lstFilePaths.indexOf("") != -1)
        {
            lstFilePaths.removeAt(lstFilePaths.indexOf(""));
        }

        emit sigSearchOver(lstFilePaths);
        break;
    case QProcess::Starting:
        lstFilePaths.clear();
    default:
        break;
    }
}
