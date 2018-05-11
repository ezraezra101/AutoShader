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

#ifndef GLWIDGET_H
#define GLWIDGET_H

// Includes from the project
#include "crossfieldgraphic.h"

// External libraries / headers (Solvers, IO, Debugging)
#include <QGLWidget>
#include <QList>
#include <QPoint>
#include <QWheelEvent>

// OpenGL window to display crossfields
class GLWidget : public QGLWidget
{
    Q_OBJECT

public:

    explicit GLWidget(QWidget *parent = 0);
    void setCrossFieldGraphic(CrossFieldGraphic * cfg);

    void activeCrossesMode();
    void activeHatchingMode();
    void activeConstraintMode();
    void updateView();
    void updateCursorConstraint();
    void updateCursorArrow();
    void updateCursorWait();

protected:

    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);

private:

    CrossFieldGraphic * crossfieldgraphic;
    QList<QPoint> pointList;
    bool registering;

    int strokeSize;
    int mode;
};

#endif // GLWIDGET_H