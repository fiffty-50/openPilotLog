#ifndef RUNGUARD_H
#define RUNGUARD_H

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>

/*!
 * \brief The RunGuard class ensures only a single instance of the application
 * is running simultaneously.
 */
class ARunGuard
{

public:
    ARunGuard(const QString &key);
    ~ARunGuard();

    bool isAnotherRunning();
    bool tryToRun();
    void release();

private:
    const QString key;
    const QString memLockKey;
    const QString sharedmemKey;

    QSharedMemory sharedMem;
    QSystemSemaphore memLock;

    Q_DISABLE_COPY(ARunGuard)
};


#endif // RUNGUARD_H
