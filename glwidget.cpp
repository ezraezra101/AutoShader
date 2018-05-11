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

#define CROSSMODE 0
#define HATCHMODE 1
#define CONSTRAINTMODE 3


GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    this->mode = true;
    registering = false;
    this->strokeSize = 5;
    QPixmap pix_cursor(":/icons/brush");
    pix_cursor = pix_cursor.scaledToHeight(20);
    QCursor cursor(pix_cursor);
    this->setCursor(cursor);
}

void GLWidget::setCrossFieldGraphic(CrossFieldGraphic * cfg)
{
    crossfieldgraphic = cfg;
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

    this->crossfieldgraphic->setDimensions(this->height(), this->width());
}

void GLWidget::updateView()
{
    this->crossfieldgraphic->setDimensions(this->height(), this->width());
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
    }

    painter.end();

}

void GLWidget::activeCrossesMode()
{
    this->mode = CROSSMODE;
    this->repaint();
}

void GLWidget::activeHatchingMode()
{
    this->mode = HATCHMODE;
    this->repaint();
}

void GLWidget::activeConstraintMode()
{
    this->mode = CONSTRAINTMODE;
    this->repaint();
}



void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        registering = true;
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(registering)
    {
        this->crossfieldgraphic->removeConstraints(event->pos(),this->rect(), this->strokeSize);
        this->repaint();
    }

}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        registering = false;
    }
}

void GLWidget::updateCursorConstraint()
{
    // create cursor
    QPixmap cursor_img(64,64);
    cursor_img.fill(Qt::transparent);
    QPainter painter(&cursor_img);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QBrush brush(QColor(255,0,0,200));
    QPen pen(QColor(0,0,0,0),0);


    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawEllipse(QPoint(32,32),this->strokeSize*2,this->strokeSize*2);
    painter.end();

    // Set it
    QCursor cursor(cursor_img,32,32);
    this->setCursor(cursor);
}


void GLWidget::updateCursorArrow()
{
    this->setCursor(Qt::ArrowCursor);
}

void GLWidget::updateCursorWait()
{
    this->setCursor(Qt::WaitCursor);
}


void GLWidget::wheelEvent(QWheelEvent* event)
{

   // get delta
   int numDegrees = event->delta() / 8;
   int numSteps = numDegrees / 15;

   // modify size of stroke
   int newSize =  this->strokeSize  + numSteps;

   // Check null size
   if(newSize < 1)
   {
       newSize = 1;
   }

   this->strokeSize = newSize;

   updateCursorConstraint();

}
