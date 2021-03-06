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

#ifndef PERIODJUMPFIELD_H
#define PERIODJUMPFIELD_H

// Includes from the project
#include "unknownsindexer.h"
#include "vec2doperations.h"
#include "labeledmap.h"

// External libraries / headers (Solvers, IO, Debugging)
#include <cv.h>
#include <highgui.h>
#include <QDebug>
#include <QList>
#include <QPoint>
#include <QRgb>
#include <QColor>
#include <QPair>
#include <QVector>
#include <iostream>
#include <cmath>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>

using namespace cv;
class UnknownsIndexer;

// This class handles crossfield period jumps
// exposed in the paper (integer field expressing
// matching between neighbouring crosses in the
// cross field)
class PeriodJumpField
{
    // Integer period jump field H x W x 2 (bottom and right neighbour p stored)
    int *** field;

    // Undefined period jumps list
    QVector<QPair<QPoint,QPoint> > * notDefinedP;

    // Dimensions
    int h,w;

    // Init the field arrays
    void createField(int,int);

public:

    // Constructor
    PeriodJumpField();

    // Constructor
    PeriodJumpField(int,int);

    // Memory disposer
    ~PeriodJumpField();

    // This algorithm is design to asign p=0 for every cell in p which is not in the border
    // between to regions of the labeled map. Border cells are queued in notDefinedP vector,
    void initialice(LabeledMap *, Mat, UnknownsIndexer *);

    // Getters and Setters
    int getUpperP(int i, int j);
    int getBottomP(int i, int j);
    int getLeftP(int i, int j);
    int getRightP(int i, int j);

    void setP(int i,int j,int index, int p);
    void setP(QPoint i,QPoint j, int p);
    void setUpperP(int i, int j, int p);
    void setBottomP(int i, int j, int p);
    void setLeftP(int i, int j, int p);
    void setRightP(int i, int j, int p);   

    QVector<QPair<QPoint,QPoint> >  * getNonDefinedPList();

    // Export and load imformation from files
    void saveToXML(QString path);
    void saveField(Mat mask);
    void loadFromXML(QString path);    
    void printField();

};

#endif // PERIODJUMPFIELD_H
