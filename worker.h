#ifndef WORKER_H
#define WORKER_H

#include "define.h"

#include <QObject>
#include <QProcess>
#include <DMessageManager>

DWIDGET_USE_NAMESPACE

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);

signals:
    void sigSearchOver(QStringList lstFilePaths);
    void sigCheckEnvRst(CheckEnvRst checkEnvRst);

public slots:
    void onSearch(QString fileName, SearchType searchType);
    void onRead();
    void onProcessStateChanged(QProcess::ProcessState state);
    void onCheckEnv();
    void onReadCheckEnv();
    void onUpdateDb(QString passwd);
    void onReadUpdateDb();

private:
    QProcess* process = nullptr;
    QProcess* processCheckEnv = nullptr;
    QProcess* processUpdateDb= nullptr;
    QStringList lstFilePaths;
};

#endif // WORKER_H
