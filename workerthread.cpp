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

    this->constraintsCounter = widget->getCanvas(GLWidget::CONSTRAINT_CANVAS).getNumInteractions();
    this->curvatureCounter = widget->getCanvas(GLWidget::CURVATURE_CANVAS).getNumInteractions();

    if(!isRunning()) {
        start(LowPriority);
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
    int consCnt = 0, curvCnt = 0;
    QImage concavityImg;
    forever {
        mutex.lock();
        if(newThread || consCnt != this->curvatureCounter || curvCnt != this->curvatureCounter) {
            worker = Worker(this->constraints, this->curvature, this->mask, this->concavity);
            newThread = false;
        } else {
            concavityImg = this->concavity;
        }
        consCnt = this->constraintsCounter;
        curvCnt = this->curvatureCounter;
        mutex.unlock();

        if(!worker.isDone())
            worker.smoothingIteration();
        else
            worker.updateConcavity(concavityImg);

        emit renderedImages(worker.drawCrosses(), worker.drawNormals(), worker.drawShading());

        mutex.lock();
        if(!restart && worker.isDone())
            condition.wait(&mutex);
        if(abort)
            return;
        restart = false;
        mutex.unlock();
    }
}
