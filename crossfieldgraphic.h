/*
Copyright or © or Copr. Emmanuel Iarussi, David Bommes, Adrien Bousseau
BendFields: Regularized Curvature Fields from Rough Concept Sketches (2015)

emmanueliarussi (AT) gmail (DOT) com
bommes (AT) aices (DOT) rwth-aachen (DOT) de
adrien (DOT) bousseau (AT) inria (DOT) fr

This software is a computer program whose purpose is to compute cross
fields over sketches using the approach especified in BendFields paper.

This software is governed by the CeCILL  license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.
*/

#ifndef CROSSFIELDGRAPHIC_H
#define CROSSFIELDGRAPHIC_H

// Includes from the project
#include "crossfield.h"
#include "normalfield.h"
#include "imageconverter.h"

// External libraries / headers (Solvers, IO, Debugging)
#include <QLinearGradient>
#include <QPaintEvent>
#include <QPen>
#include <QFont>
#include <QPainter>
#include <QImage>
#include <QTime>
#include <QList>
#include <QPoint>
#include <cv.h>
#include <highgui.h>


using namespace cv;

// This class renders a crossfield, representing it
// as sparse little crosses or using a line integral
// convolution implementation
class CrossFieldGraphic
{
public:

    CrossFieldGraphic();
    void paint(QPainter * painter, QPaintEvent *event);
    void paintCrosses(QPainter *painter, QPaintEvent *event);
    void paintConstraints(QPainter *painter, QPaintEvent *event);
    void paintNormals(QPainter *painter, QPaintEvent *event);
    void setCrossField(CrossField *);
    void setMask(Mat);
    void setCornersMask(Mat m);
    void setSketch(Mat);
    void setShading(Mat m);
    void setStep(int);    
    void removeConstraints(QPoint points, QRect rect, int size);
    void setDimensions(int h, int w);
    void setEraseState(bool);

private:

    QList<QPair<double,double> > findNeighbour(int i, int j, int w, int h, int x_despl, int y_despl, bool orientation, int lineLong);
    void drawLines(QPainter * painter, QList<QPair<double,double> > neighbours, int x_despl, int y_despl, int bWidth, int bHeight, QPainter * paintToFile);

    QColor background;

    CrossField * crossfield;

    Mat noCornersImage;

    QRect repaintArea;

    QImage * mask;
    QImage * sketch;
    QImage * constraintsBuffer;
    QImage * shading;

    int step,glWidgetHeight,glWidgetWidth,sketchScaledHeight,sketchScaledWidth,x_displ,y_displ;
    bool eraseToolActive;
};

#endif // CROSSFIELDGRAPHIC_H
