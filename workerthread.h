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

    void makeCrossField(QImage constraints, QImage curvature, QImage mask, GLWidget * dummy); // TODO remove dummy

    QImage getNormals(CrossField *crossfield);

    QImage drawCrosses(CrossField &cf);
};

#endif // WORKERTHREAD_H
