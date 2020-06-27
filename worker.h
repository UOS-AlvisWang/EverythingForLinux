#ifndef WORKER_H
#define WORKER_H

#include "define.h"

#include <QObject>
#include <QProcess>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = 0);

signals:
    void sigSearchOver(QStringList lstFilePaths);
    void sigCheckEnvRst(CheckEnvRst checkEnvRst);

public slots:
    void onSearch(QString fileName, SearchType searchType);
    void onRead();
    void onProcessStateChanged(QProcess::ProcessState state);
    void onCheckEnv();
    void onReadCheckEnv();

private:
    QProcess* process = nullptr;
    QProcess* processCheckEnv = nullptr;
    QStringList lstFilePaths;
};

#endif // WORKER_H
