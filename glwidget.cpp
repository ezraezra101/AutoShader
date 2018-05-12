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

#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    this->mode = CONSTRAINT_CANVAS;

    int x = 600, y = 400;
    constraintStrokes = Canvas(); constraintStrokes.init(x,y);
    curvatureStrokes = Canvas(); curvatureStrokes.init(x,y);
    shading = Canvas(); shading.init(x,y);
    mask = Canvas(); mask.init(x,y);

    activeCanvas().updateCursor(this);
}

void GLWidget::setCrossFieldGraphic(CrossFieldGraphic * cfg)
{
    //crossfieldgraphic = cfg; // TODO reimplement
}

void GLWidget::initializeGL()
{
    // Screen color
    this->qglClearColor(QColor(255,255,255));

    glClearColor(0.0, 0.0, 102.0/255.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, (GLint)width, (GLint)height);

    activeCanvas().updateCursor(this);
}

void GLWidget::updateView()
{
}


void GLWidget::paintGL()
{

    glClear(GL_COLOR_BUFFER_BIT);

    glFlush();
}


void GLWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    /*

    if(mode==CROSSMODE)
    {
        crossfieldgraphic->paintCrosses(&painter, event);
    }
    else if(mode==HATCHMODE)
    {
        crossfieldgraphic->paint(&painter, event);

    }
    else if(mode==CONSTRAINTMODE)
    {
        crossfieldgraphic->paintConstraints(&painter, event);
    } else if(mode==NORMALMODE)
    {
        crossfieldgraphic->paintNormals(&painter, event);
    }
*/
    curvatureStrokes.draw(&painter, event, this);
    mask.draw(&painter, event, this);
    shading.draw(&painter, event, this);
    constraintStrokes.draw(&painter, event, this);
    painter.end();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
//    if(event->button()==Qt::LeftButton)
//    {
//        registering = true;
//    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    activeCanvas().mouseEvent(event, this);
//    if(registering)
//    {
//        this->crossfieldgraphic->removeConstraints(event->pos(),this->rect(), this->strokeSize);
//        this->repaint();
//    }

}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
//    if(event->button()==Qt::LeftButton)
//    {
//        registering = false;
//    }
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
    activeCanvas().mouseWheelEvent(event, this);
}

void GLWidget::setActiveCanvas(GLWidget::CanvasEnum mode) {
    this->mode = mode;
    activeCanvas().updateCursor(this);
}

Canvas &GLWidget::activeCanvas() {
    return getCanvas(mode);
}

Canvas &GLWidget::getCanvas(CanvasEnum c) {
    switch(c) {
    case CONSTRAINT_CANVAS:
        return constraintStrokes;
    case CURVATURE_CANVAS:
        return curvatureStrokes;
    case MASK_CANVAS:
        return mask;
    default:
        return constraintStrokes;
    }
}
