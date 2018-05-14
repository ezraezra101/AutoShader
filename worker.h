#ifndef WORKER_H
#define WORKER_H

#include <cv.h>
#include <QImage>

#include "crossfield.h"
#include "crossfield3d.h"
#include "harmoniccrossfield.h"
#include "bendfield.h"
#include "normalfield.h"
#include "imageconverter.h"
#include "distancetransform.h"

class Worker
{

public:
    Worker();
    Worker(QImage constraints, QImage curvature, QImage mask, QImage concavity);
    ~Worker();

    // perform a single iteration of smoothing and prepare for some kind of output.
    void smoothingIteration();
    bool isDone() const;
    void updateConcavity(QImage concavity);
    QImage drawNormals();
    QImage drawShading();
    QImage drawCrosses(int step = 25);

private:

    void initializeCrossField();
    void rotateCrosses();
    void makeCrossField3d(const QImage &concavity);
    QImage maskTheImage(QImage);

    bool hasCrossField() const;
    bool hasCrossField3d() const;

    QImage constraints, curvature, maskImg, concavity;

    CrossField *crossfield;
    PeriodJumpField *pjumpfield;
    UnknownsIndexer *index_harmonic;
    HarmonicCrossField *harmoniccrossfield;
    UnknownsIndexer *index_covariant;
    BendField *bendfield;
    cv::Mat mask;
    CrossField3D * crossfield3d;
};

#endif // Worker_H
