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

#include "crossfieldgraphic.h"

//#define LINE_LONG 50
#define STEP 20
#define VSIZE 10

CrossFieldGraphic::CrossFieldGraphic()
{
    background = QColor(255, 255, 255);

    this->sketch = NULL;
    this->mask = NULL;
    this->crossfield = NULL;
    this->shading = NULL;
    this->step = 20;
    repaintArea.setWidth(0);
    repaintArea.setHeight(0);
    constraintsBuffer = new QImage(0,0,QImage::Format_ARGB32_Premultiplied);
    constraintsBuffer->fill(Qt::transparent);
    eraseToolActive = false;
}

void CrossFieldGraphic::setCrossField(CrossField  *cf)
{
    this->crossfield = cf;
}

void CrossFieldGraphic::setMask(Mat m)
{
    Mat * newMat = new Mat();
    m.copyTo(*newMat);
    this->mask = ImageConverter::toQImage(newMat);

}


void CrossFieldGraphic::setShading(Mat m)
{
    Mat * newMat = new Mat();
    m.copyTo(*newMat);
    this->shading = ImageConverter::toQImage(newMat);
}

void CrossFieldGraphic::setCornersMask(Mat m)
{
    this->noCornersImage = m;
}    

void CrossFieldGraphic::setStep(int s)
{
    this->step = s;
}

void CrossFieldGraphic::setSketch(Mat s)
{
    Mat * newMat = new Mat();
    s.copyTo(*newMat);
    this->sketch = ImageConverter::toQImage(newMat);    
}

void CrossFieldGraphic::setDimensions(int h, int w)
{
    this->glWidgetHeight = h;
    this->glWidgetWidth = w;

    this->sketchScaledHeight =  this->glWidgetHeight;
    this->sketchScaledWidth = this->glWidgetWidth;

    if(this->sketch!= NULL)
    {
        // Check bigger dim
        if(this->sketch->height()>this->sketch->width())
        {
            // Save new scales
            this->sketchScaledWidth = this->sketch->width()*((double)((double)this->glWidgetHeight/(double)this->sketch->height()));
            this->x_displ = (double)((this->glWidgetWidth - this->sketchScaledWidth) / 2.);
            this->y_displ = 0;
        }
        else
        {
            // Save new scales
            this->sketchScaledHeight = this->sketch->height()*((double)((double)this->glWidgetWidth/(double)this->sketch->width()));
            this->x_displ = 0;
            this->y_displ = (double)((this->glWidgetHeight - this->sketchScaledHeight) / 2.);
        }

        if(this->constraintsBuffer!= NULL)
        {
            delete this->constraintsBuffer;
        }
        this->constraintsBuffer = new QImage(this->sketchScaledWidth,this->sketchScaledHeight,QImage::Format_ARGB32_Premultiplied);
        this->constraintsBuffer->fill(Qt::transparent);
        repaintArea.setWidth(0);
        repaintArea.setHeight(0);

    }


}

void CrossFieldGraphic::setEraseState(bool state)
{
    eraseToolActive = state;

}

void CrossFieldGraphic::removeConstraints(QPoint point, QRect rect, int size)
{

    // Convert to image coordinates
    int j = ((point.x()-x_displ)*this->sketch->width())/this->sketchScaledWidth;
    int i = ((point.y()-y_displ)*this->sketch->height())/this->sketchScaledHeight;

    // Mark pixels as curvature points
    for(int k = i-size; k < i+size; k++)
    {
        if((k>0)&&(k < this->crossfield->height()))
        {
            for(int l = j-size; l < j+size; l++)
            {
                if((l>0)&&(l < this->crossfield->width()))
                {               
                     this->crossfield->markAsCurvatureLine(k,l);
                }
            }
        }
    }

    // Check repaint area is inside the image limits
    int left_margin = j-size;
    int top_margin = i-size;
    int width_repaint = size*2;
    int height_repaint = size*2;

    if((left_margin)<0)
    {
        left_margin = 0;
    }
    if((top_margin)<0)
    {
        top_margin = 0;
    }
    if((left_margin+width_repaint)>=this->crossfield->width())
    {
        width_repaint = width_repaint - (left_margin + width_repaint - this->crossfield->width());
    }
    if((top_margin+height_repaint)>=this->crossfield->height())
    {
        height_repaint = height_repaint - (top_margin + height_repaint - this->crossfield->height());
    }

    // Set repaint area
    this->repaintArea.setX(left_margin);
    this->repaintArea.setY(top_margin);
    this->repaintArea.setWidth(width_repaint);
    this->repaintArea.setHeight(height_repaint);

}

QList<QPair<double,double> > CrossFieldGraphic::findNeighbour(int i, int j, int w, int h, int x_despl, int y_despl, bool orientation, int lineLong)
{
    // The list of line neighbourds (p0,p1,p2..,pi) with i = k
    QList<QPair<double,double> > neighbours;

    // The center of the stream line neighbour. The i,j directions in the image are fliped to match the x,y axis.
    double x = i;
    double y = j;

    // fx, fy are the internal pixel deplacement of the stream line. We start in the midle of a pixel (0.5,0.5)
    double fx = 0.5;
    double fy = 0.5;


    Vec2d v0 = crossfield->getV0((int)(((y-y_despl)*this->sketch->height())/h),(int)(((x-x_despl)*this->sketch->width())/w));
    Vec2d v1 = crossfield->getV1((int)(((y-y_despl)*this->sketch->height())/h),(int)(((x-x_despl)*this->sketch->width())/w));


    // Direction selection
    double direction;

    if(orientation == 0)
    {
        if(v0(1)>=0)
        {
            direction = fmod(acos(v0(0)),M_PI);
        }
        else
        {
            direction = fmod((2*M_PI) - acos(v0(0)),M_PI);
        }
    }
    else
    {
        if(v1(1)>=0)
        {
            direction = fmod(acos(v1(0)),M_PI);
        }
        else
        {
            direction = fmod((2*M_PI) - acos(v1(0)),M_PI);
        }
    }

    double last_direction = direction;

    // Push the center into the results (y,x)
    neighbours.push_back(QPair<double,double>(x,y));

    // Add neighbours (positive direction)
    for(int l = 0; l < lineLong; l++)
    {
        // Take the last pixel added (current position)
        QPair<double,double> pi = neighbours.last();

        // Check if we are inside the image bounds still.
        if((pi.first-x_despl < w)&&(pi.first-x_despl > 0.0)&&(pi.second-y_despl < h)&&(pi.second-y_despl > 0.0))
        {
            // Do not propagate across strokes
            if(!((this->crossfield->isBoundaryPixel((int)(((y-y_despl)*this->sketch->height())/h),(int)(((x-x_despl)*this->sketch->width())/w)))
              &&(!this->crossfield->isCurvatureLine((int)(((y-y_despl)*this->sketch->height())/h),(int)(((x-x_despl)*this->sketch->width())/w)))))
            {

            Vec2d v0 = crossfield->getV0((int)(((y-y_despl)*this->sketch->height())/h),(int)(((x-x_despl)*this->sketch->width())/w));
            Vec2d v1 = crossfield->getV1((int)(((y-y_despl)*this->sketch->height())/h),(int)(((x-x_despl)*this->sketch->width())/w));


            // Set direction, by checking last_direction
            // The 2 candidates

            double candidate_direction_1;
            double candidate_direction_2 ;

            if(v0(1)>=0)
            {
                candidate_direction_1 = fmod(acos(v0(0)),M_PI);
            }
            else
            {
                candidate_direction_1 = fmod((2*M_PI) - acos(v0(0)),M_PI);
            }

            if(v1(1)>=0)
            {
                candidate_direction_2 = fmod(acos(v1(0)),M_PI);
            }
            else
            {
                candidate_direction_2 = fmod((2*M_PI) - acos(v1(0)),M_PI);
            }

            // In the new cross selected, select the closest vetor to the previous one
            if((pow(last_direction-candidate_direction_1,2)<pow(last_direction-candidate_direction_2,2))&&(abs(last_direction-candidate_direction_1)<(double)(M_PI/4)))
            {
                direction = candidate_direction_1;
            }
            else if (abs(last_direction-candidate_direction_2)<(double)(M_PI/4))
            {
                direction = candidate_direction_2;

            }
            else
            {
                break;
            }

            // After the election, last is current
            last_direction = direction;


            Vec2d vpi(cos(direction),sin(direction));

            // Normalized
            Vec2d vpiNorm = Vec2dOperations::getNormalized(vpi);

            // Vx and Vy are the directions of the next movement.
            // We stil need to decide if we do a step in the x axis or in the y axis.
            double vx =vpiNorm[0];
            double vy =vpiNorm[1];

            // Tx represents the relationship between the current position inside the pixel (fx)
            // and the magnitud of the deplacement in x (vx). The same for ty.
            double tx = 0;
            double ty = 0;

            // Check if at least one of them is != 0. If one of them is cero, doesn't mather
            // f will be infinite, and we will choose to move in the other direction.
            if((vx!=0)||(vy!=0))
            {
                // Compute fx and fy. It must be always a positive factor.
                if(vx>=0)
                    tx = (1-fx)/vx;
                else
                    tx = -fx/vx;
                if(vy>=0)
                    ty = (1-fy)/vy;
                else
                    ty = -fy/vy;

                // The smaller one is the direction we must follow.
                // Big vx means we want to move in x direction.
                // Big vy means we want to move in y direction.
                // Small (or 0) vx means an infinite tx, we want to move in the y direction.
                // Small (or 0) vy means an infinite ty, we want to move in the x direction.

                if(tx<ty)
                {
                    // qDebug() << "x step";
                    // Go to the right
                    if(vx>0)
                    {
                       x+=1;
                       fy+=vy*tx;
                       fx=0;
                    }
                    // Go to the left
                    else
                    {
                       x-=1;
                       fy+=vy*tx;
                       fx=1;
                    }
                }
                else
                {
                    // qDebug() << "y step";
                    // Go up
                    if(vy>0)
                    {
                       y+=1;
                       fx+=vx*ty;
                       fy=0;
                    }
                    // Go down
                    else
                    {
                       y-=1;
                       fx+=vx*ty;
                       fy=1;
                    }
                }

                // If we are still inside te image.
                if(((int)y-y_despl < h)&&((int)y-y_despl > 0)&&((int)x-x_despl < w)&&((int)x-x_despl > 0))
                {
                    if(!neighbours.contains(QPair<double,double>(x,y)))
                    {
                        // Add it to the list
                        neighbours.push_back(QPair<double,double>(x,y));

                    }
                    else
                    {
                        // Stop the cicle (we are in a loop)
                        l = lineLong;

                    }
                }
                else
                {
                    // Stop the cicle (out of image)
                    l = lineLong;

                }
            }
            else
            {
                // Stop the cicle (vx and vy = 0 in that point. NO flow.)
                l = lineLong;
            }
        }
        }
    }

    // Reset variables for moving in th other direction
    fx = 0.5;
    fy = 0.5;
    x = i;
    y = j;

    // Add neighbours (negative direction)
    for(int l = 0; l < lineLong; l++)
    {
        // Take the last pixel added, now on the front of the list (current position)
        QPair<double,double> pi = neighbours.first();

        // Check if we are inside the image bounds still.
        if((pi.first-x_despl < w)&&(pi.first-x_despl > 0.0)&&(pi.second-y_despl < h)&&(pi.second-y_despl > 0.0))
        {
            // Do not propagate across strokes
            if(!((this->crossfield->isBoundaryPixel((int)(((y-y_despl)*this->sketch->height())/h),(int)(((x-x_despl)*this->sketch->width())/w)))
              &&(!this->crossfield->isCurvatureLine((int)(((y-y_despl)*this->sketch->height())/h),(int)(((x-x_despl)*this->sketch->width())/w)))))
            {
            Vec2d v0 = crossfield->getV0((int)(((y-y_despl)*this->sketch->height())/h),(int)(((x-x_despl)*this->sketch->width())/w));
            Vec2d v1 = crossfield->getV1((int)(((y-y_despl)*this->sketch->height())/h),(int)(((x-x_despl)*this->sketch->width())/w));



            // Set direction, by checking last_direction
            // The 2 candidates
            double candidate_direction_1;
            double candidate_direction_2 ;

            if(v0(1)>=0)
            {
                candidate_direction_1 = fmod(acos(v0(0)),M_PI);
            }
            else
            {
                candidate_direction_1 = fmod((2*M_PI) - acos(v0(0)),M_PI);
            }

            if(v1(1)>=0)
            {
                candidate_direction_2 = fmod(acos(v1(0)),M_PI);
            }
            else
            {
                candidate_direction_2 = fmod((2*M_PI) - acos(v1(0)),M_PI);
            }
            // In the new cross selected, select the closest vetor to the previous one
            if((pow(last_direction-candidate_direction_1,2)<pow(last_direction-candidate_direction_2,2))&&(abs(last_direction-candidate_direction_1)<(double)(M_PI/4)))
            {
                direction = candidate_direction_1;
            }
            else  if (abs(last_direction-candidate_direction_2)<(double)(M_PI/4))
            {
                direction = candidate_direction_2;
            }
            else
            {

                break;
            }


            // After the election, last is current
            last_direction = direction;


            Vec2d vpi(cos(direction),sin(direction));

            // Normalized
            Vec2d vpiNorm = Vec2dOperations::getNormalized(vpi);

            // Vx and Vy are the directions of the next movement.
            // We stil need to decide if we do a step in the x axis or in the y axis.
            double vx =vpiNorm[0];
            double vy =vpiNorm[1];

            // Tx represents the relationship between the current position inside the pixel (fx)
            // and the magnitud of the deplacement in x (vx). The same for ty.
            double tx = 0;
            double ty = 0;

            // Check if at least one of them is != 0. If one of them is cero, doesn't mather
            // f will be infinite, and we will choose to move in the other direction.
            if((vx!=0)||(vy!=0))
            {
                // Compute fx and fy. It must be always a positive factor.
                if(vx>=0)
                    tx = (1-fx)/vx;
                else
                    tx = -fx/vx;
                if(vy>=0)
                    ty = (1-fy)/vy;
                else
                    ty = -fy/vy;

                // The smaller one is the direction we must follow.
                // Big vx means we want to move in x direction.
                // Big vy means we want to move in y direction.
                // Small (or 0) vx means an infinite tx, we want to move in the y direction.
                // Small (or 0) vy means an infinite ty, we want to move in the x direction.

                if(tx<ty)
                {
                    //qDebug() << "x step";
                     // Go to the left
                    if(vx>0)
                    {
                       x-=1;
                       fy+=vy*tx;
                       fx=0;
                    }
                    else
                    {
                       // Go to the right
                       x+=1;
                       fy+=vy*tx;
                       fx=1;
                    }
                }
                else
                {
                    //qDebug() << "y step";
                    // Go down
                    if(vy>0)
                    {
                       y-=1;
                       fx+=vx*ty;
                       fy=0;
                    }
                    // Go down
                    else
                    {
                       y+=1;
                       fx+=vx*ty;
                       fy=1;
                    }
                }

                // If we are still inside te image.
                if(((int)y-y_despl < h)&&((int)y-y_despl > 0)&&((int)x-x_despl < w)&&((int)x-x_despl > 0))
                {
                    if(!neighbours.contains(QPair<double,double>(x,y)))
                    {
                        // Add it to the list
                        neighbours.push_front(QPair<double,double>(x,y));
                    }
                    else
                    {
                        // Stop the cicle (we are in a loop)
                        l = lineLong;
                    }
                }
                else
                {
                     // Stop the cicle (out of image)
                    l = lineLong;
                }
            }
            else
            {
                // Stop the cicle (vx and vy = 0 in that point. NO flow.)
                l = lineLong;
            }
        }
        }
    }

    return neighbours;
}

void CrossFieldGraphic::drawLines(QPainter * painter, QList<QPair<double,double> > neighbours,  int x_despl, int y_despl,  int bWidth, int bHeight,QPainter * paintToFile)
{

    // Averaging using the kernel
    for(int i = 0; i < neighbours.size()-2; i++)
    {

        double x = neighbours.at(i).first; // over cols
        double y = neighbours.at(i).second; // over rows
        double x_1 = neighbours.at(i+1).first;
        double y_1 = neighbours.at(i+1).second;

        // access mask (col, row) format - scale and remove desplacement

        QRgb maskColor = this->mask->pixel(((x-x_despl)*this->sketch->width())/bWidth,((y-y_despl)*this->sketch->height())/bHeight);
        QRgb maskColor_1 = this->mask->pixel(((x_1-x_despl)*this->sketch->width())/bWidth,((y_1-y_despl)*this->sketch->height())/bHeight);

        if((maskColor != QColor(0,0,0).rgb())&&(maskColor_1 != QColor(0,0,0).rgb()))
        {           
            painter->drawLine(neighbours.at(i).first,neighbours.at(i).second,neighbours.at(i+1).first,neighbours.at(i+1).second);           
            paintToFile->drawLine(neighbours.at(i).first,neighbours.at(i).second,neighbours.at(i+1).first,neighbours.at(i+1).second);
        }

    }

}


void CrossFieldGraphic::paint(QPainter *painter, QPaintEvent *event)
{
    // refix random seed
    srand (12);

    // Fill it white
    painter->fillRect(event->rect(), background);

    // If sketch
    if(this->sketch != NULL)
    {

        QImage backgroundImage;

        backgroundImage = this->sketch->scaled(this->sketchScaledWidth,this->sketchScaledHeight,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        QImage toSave(painter->window().width(),painter->window().height(),QImage::Format_ARGB32);
        toSave.fill(Qt::white);
        QPainter * paintToFile = new QPainter(&toSave);
        paintToFile->setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform);

        QRect postion(x_displ,y_displ,this->sketchScaledWidth,this->sketchScaledHeight);

        painter->drawImage(postion,backgroundImage);


        // If crossfield
        if(this->crossfield != NULL)
        {
            // 100 samples of it            
            for(int k = 0; k<(step*200); k++)
            {
                // random positon (not in the borders)
                int i = (rand() % (backgroundImage.width()))+x_displ;
                int j = (rand() % (backgroundImage.height()))+y_displ;

                QRgb maskColor = this->mask->pixel((((i-x_displ)*this->sketch->height())/backgroundImage.height()),(((j-y_displ)*this->sketch->width())/backgroundImage.width()));

                if(maskColor != QColor(0,0,0).rgb())
                {
                    // random orientation
                    bool orientation = rand() % 2;

                    // random line lenght
                    int lenght = 50;

                    // Find its line neighbours
                    QList<QPair<double,double> > neighbours = findNeighbour(i,j,backgroundImage.width(),backgroundImage.height(), x_displ, y_displ, orientation, lenght);

                    // Draw Lines
                    drawLines(painter, neighbours, x_displ, y_displ, backgroundImage.width(),backgroundImage.height(),paintToFile);
                }
                else
                {
                    k++;
                }
            }
        }

        paintToFile->end();
        toSave.save("output/hatching.png");
    }
}

void CrossFieldGraphic::paintCrosses(QPainter *painter, QPaintEvent *event)
{
    // Fill it white
    painter->fillRect(event->rect(), background);

    // If sketch
    if(this->sketch != NULL)
    {
        QImage backgroundImage;

        backgroundImage = this->sketch->scaled(this->sketchScaledWidth,this->sketchScaledHeight,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        QRect postion(x_displ,y_displ,this->sketchScaledWidth,this->sketchScaledHeight);
        painter->drawImage(postion,backgroundImage);

        QPen pen(QColor(0,0,0), 3);
        painter->setPen(pen);

        // If crossfield
        if(this->crossfield != NULL)
        {
            // For all the pixels
            for(int i = 0; i < backgroundImage.height(); i = i+step)
            {
                for(int j = 0; j < backgroundImage.width(); j = j+step)
                {
                    QRgb maskColor = this->mask->pixel((((j)*this->sketch->width())/backgroundImage.width()),(((i)*this->sketch->height())/backgroundImage.height()));

                    if(maskColor != QColor(0,0,0).rgb())
                    {                       
                        // Draw cross
                        double y = i;
                        double x = j;

                         if(!crossfield->isBoundaryPixel((int)(((y)*this->sketch->height())/backgroundImage.height()),(int)(((x)*this->sketch->width())/backgroundImage.width())))
                         {
                            Vec2d v_0 = crossfield->getV0((int)(((y)*this->sketch->height())/backgroundImage.height()),(int)(((x)*this->sketch->width())/backgroundImage.width()));
                            Vec2d v_1 = crossfield->getV1((int)(((y)*this->sketch->height())/backgroundImage.height()),(int)(((x)*this->sketch->width())/backgroundImage.width()));
                            Vec2d v_2 = crossfield->getV2((int)(((y)*this->sketch->height())/backgroundImage.height()),(int)(((x)*this->sketch->width())/backgroundImage.width()));
                            Vec2d v_3 = crossfield->getV3((int)(((y)*this->sketch->height())/backgroundImage.height()),(int)(((x)*this->sketch->width())/backgroundImage.width()));

                            Vec2d v_0_norm = v_0;//Vec2dOperations::getNormalized(v_0);
                            Vec2d v_1_norm = v_1;//Vec2dOperations::getNormalized(v_1);
                            Vec2d v_2_norm = v_2;//Vec2dOperations::getNormalized(v_2);
                            Vec2d v_3_norm = v_3;//Vec2dOperations::getNormalized(v_3);


                            painter->drawLine(x+x_displ,y+y_displ,x+x_displ+(v_0_norm(0))*(step/2),y+y_displ+(v_0_norm(1))*(step/2));
                            painter->drawLine(x+x_displ,y+y_displ,x+x_displ+(v_1_norm(0))*(step/2),y+y_displ+(v_1_norm(1))*(step/2));
                            painter->drawLine(x+x_displ,y+y_displ,x+x_displ+(v_2_norm(0))*(step/2),y+y_displ+(v_2_norm(1))*(step/2));
                            painter->drawLine(x+x_displ,y+y_displ,x+x_displ+(v_3_norm(0))*(step/2),y+y_displ+(v_3_norm(1))*(step/2));
                         }
                    }
                }
            }
        }
    }
}

void CrossFieldGraphic::paintConstraints(QPainter *painter, QPaintEvent *event)
{

    // Fill it white
    painter->fillRect(event->rect(), background);

    // area
    int min_i, min_j, max_i, max_j, repaint_height, repaint_width;

    if((this->repaintArea.width()==0)&&(this->repaintArea.height()==0))
    {
        this->constraintsBuffer->fill(Qt::transparent);
        min_j = 0;
        min_i = 0;
        max_j = this->sketch->width();
        max_i = this->sketch->height();

        repaint_height = this->sketch->height();
        repaint_width = this->sketch->width();

    }
    else
    {
        min_j = this->repaintArea.x();
        min_i = this->repaintArea.y();
        max_j = this->repaintArea.x()+this->repaintArea.width();
        max_i = this->repaintArea.y()+this->repaintArea.height();
        repaint_height = this->repaintArea.height();
        repaint_width = this->repaintArea.width();

    }

    // If sketch
    if(this->sketch != NULL)
    {
        QImage backgroundImage;
        backgroundImage = this->sketch->scaled(this->sketchScaledWidth,this->sketchScaledHeight,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        backgroundImage.convertToFormat(QImage::Format_ARGB32_Premultiplied);
        QRect postion(x_displ,y_displ,this->sketchScaledWidth,this->sketchScaledHeight);

        QPainter auxPainter(constraintsBuffer);

        QColor paintAngle;

        //Paint only constraints
        for(int i = min_i; i < max_i; i++)
        {
            for(int j = min_j; j < max_j; j++)
            {
                double y = (((i)*backgroundImage.height())/this->sketch->height());
                double x = (((j)*backgroundImage.width())/this->sketch->width());


                if(this->crossfield->isCurvatureLine(i,j))
                {
                    paintAngle.setRgba(QColor(255,0,0,255).rgba());
                    QPen anglePen(paintAngle, 4);
                    auxPainter.setPen(anglePen);

                    auxPainter.drawLine(x,y,x,y);

                }
            }
        }


        painter->setCompositionMode(QPainter::CompositionMode_Source);
        painter->drawImage(postion,backgroundImage);

        painter->setCompositionMode(QPainter::CompositionMode_Plus);
        painter->drawImage(postion,*constraintsBuffer);

    }

}
