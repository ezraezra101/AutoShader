#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <cv.h>
#include <QImage>

#include "crossfield.h"
#include "crossfield3d.h"
#include "normalfield.h"
#include "glwidget.h"
#include "imageconverter.h"

class WorkerThread
{
public:
    WorkerThread();
    ~WorkerThread();

    void makeCrossField(QImage constraints, QImage curvature, QImage mask, GLWidget * dummy); // TODO remove dummy

    void outputImages(GLWidget *w);

    QImage drawNormals(CrossField *crossfield);
    QImage drawShading(CrossField *crossfield);
    QImage drawCrosses(CrossField *cf);

private:
    void initializeCrossField(QImage constraintsImg, QImage curvatureImg, QImage maskImg);
    void harmonicSmoothing();
    void covariantSmoothing();
    void rotateCrosses();

    CrossField *crossfield;
    PeriodJumpField *pjumpfield;
    cv::Mat mask;

};

#endif // WORKERTHREAD_H
