#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>
#include <QWaitCondition>

#include "worker.h"

class GLWidget;

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    explicit WorkerThread(QObject *parent = nullptr);
    ~WorkerThread();

    void update(GLWidget *widget);

signals:
    void renderedImages(const QImage &, const QImage &, const QImage &);
public slots:
    void finishImages(const QImage &, const QImage &, const QImage &);
protected:
    void run() override;

private:
    QMutex mutex;
    QWaitCondition condition;
    QImage constraints, curvature, mask, concavity;
    int constraintsCounter, curvatureCounter;
    bool restart;
    bool abort;

    GLWidget *widget;
};

#endif // WORKERTHREAD_H
