#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <cv.h>
#include <QImage>

#include "crossfield.h"
#include "glwidget.h"

class WorkerThread
{

public:
    WorkerThread();

    void makeCrossField(QImage constraints, QImage curvature, QImage mask, GLWidget * dummy); // TODO remove dummy

    // Converts between QImage and Mat, using RGB888 format (looses transparency)
    // I should be using ImageConverter, but I didn't want to mess with pointers.
    QImage matToQImage(cv::Mat &mat);
    cv::Mat qImageToMat(QImage &img);


    QImage drawCrosses(CrossField &cf);
};

#endif // WORKERTHREAD_H
