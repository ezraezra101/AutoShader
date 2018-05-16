#include "workerthread.h"
#include "glwidget.h"

#define STATE_INIT_CROSSFIELD 0
#define STATE_HARMONIC_SMOOTH 1
#define STATE_BENDFIELD_SMOOTH 2
#define STATE_SLEEP 3
#define STATE_WAKE 4

WorkerThread::WorkerThread(QObject *parent) : QThread(parent)
{
    constraintsCounter = 0;
    curvatureCounter = 0;
    restart = false;
    abort = false;

    connect(this,SIGNAL(renderedImages(QImage,QImage,QImage)),this, SLOT(finishImages(QImage,QImage,QImage)));
}

WorkerThread::~WorkerThread() {
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();
    wait();
}

void WorkerThread::update(GLWidget *widget) {
    QMutexLocker locker(&mutex);
    this->widget = widget;
    this->constraints = widget->getCanvas(GLWidget::CONSTRAINT_CANVAS).getImage();
    this->curvature = widget->getCanvas(GLWidget::CURVATURE_CANVAS).getImage();
    this->mask = widget->getCanvas(GLWidget::MASK_CANVAS).getImage();
    this->concavity = widget->getCanvas(GLWidget::CONCAVITY_CANVAS).getImage();

    int consCnt = widget->getCanvas(GLWidget::CONSTRAINT_CANVAS).getNumInteractions();
    int curvCnt = widget->getCanvas(GLWidget::CURVATURE_CANVAS).getNumInteractions();
    if(consCnt != this->constraintsCounter || curvCnt != this->curvatureCounter) {
        this->resetOptimization = true;
    }
    this->constraintsCounter = consCnt;
    this->curvatureCounter = curvCnt;

    if(!isRunning()) {
        start(HighPriority);
    } else {
        restart = true;
        condition.wakeOne();
    }
}


void WorkerThread::finishImages(const QImage &crosses, const QImage &normals, const QImage &shading) {
    this->widget->getCanvas(GLWidget::CROSSFIELD_CANVAS).setImage(crosses);
    this->widget->getCanvas(GLWidget::NORMALS_CANVAS).setImage(normals);
    this->widget->getCanvas(GLWidget::SHADING_CANVAS).setImage(shading);
    this->widget->repaint();
}


void WorkerThread::run() {
    bool newThread = true;
    Worker worker;
    QImage concavityImg;
    QTime time, temp;
    forever {
        temp.restart();
        mutex.lock();
        if(this->resetOptimization || newThread) {
            worker = Worker(this->constraints, this->curvature, this->mask, this->concavity);
            newThread = false;
            this->resetOptimization = false;
            time.start();
        } else {
            concavityImg = this->concavity;
        }
        mutex.unlock();

        qDebug() << temp.restart() << "ms in mutex locked";

        if(!worker.isDone())
            worker.smoothingIteration();
        qDebug() << temp.restart() << "ms in smoothing";

        if(time.elapsed() > 5000) { // Draw at most every 5 seconds
            worker.updateConcavity(concavityImg);
            emit renderedImages(worker.drawCrosses(), worker.drawNormals(), worker.drawShading());
            time.start();
        }

        qDebug() << temp.restart() << "ms drawing";

        mutex.lock();
        if(!restart && worker.isDone()) {
            condition.wait(&mutex);
        }
        if(abort)
            return;
        restart = false;
        mutex.unlock();
    }
}
