#include "workerthread.h"

#include "crossfield.h"
#include "harmoniccrossfield.h"
#include "bendfield.h"
#include "periodjumpfield.h"
#include "distancetransform.h"
#include "unknownsindexer.h"

#define DEBUG_PRINT(str) { std::cout << ">\t" << (str) << std::endl; }


WorkerThread::WorkerThread()
{
    this->crossfield = NULL;
    this->pjumpfield = NULL;
}
WorkerThread::~WorkerThread() {
    if(this->crossfield != NULL) {
        delete this->crossfield;
    }
    if(this->pjumpfield != NULL) {
        delete this->pjumpfield;
    }
}

void WorkerThread::outputImages(GLWidget *widget) {
    if(this->crossfield != NULL) {
        widget->getCanvas(GLWidget::CROSSFIELD_CANVAS).setImage(drawCrosses(this->crossfield));
        widget->getCanvas(GLWidget::NORMALS_CANVAS).setImage(drawNormals(this->crossfield));
        widget->getCanvas(GLWidget::SHADING_CANVAS).setImage(drawShading(this->crossfield));
        widget->repaint();
    }
}

void WorkerThread::makeCrossField(QImage constraints, QImage curvature, QImage mask, GLWidget *dummy)
{

    initializeCrossField(constraints, curvature, mask);
        outputImages(dummy);

   std::cout << "Crossfield dimensions: " << crossfield->height() << "," << crossfield->width() <<std::endl;
   std::cout << "Image dimensions: " << curvature.height() << "," << curvature.width() <<std::endl;
   QImage normals = drawNormals(this->crossfield);
   std::cout << "Normal image dimensions: " << normals.height() << "," << normals.width() <<std::endl;
    return;
    harmonicSmoothing();
        outputImages(dummy);
    covariantSmoothing();
        outputImages(dummy);
    rotateCrosses();
}


void WorkerThread::initializeCrossField(QImage constraintsImg, QImage curvatureImg, QImage maskImg) {
    if(this->pjumpfield != NULL) {
        delete this->pjumpfield;
    }
    if(this->crossfield != NULL) {
        delete this->crossfield;
    }

    // Existing code expects constraints and curvature to be combined...
    QPainter p(&constraintsImg);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOver); // probably not too important
    p.drawImage(0,0,curvatureImg);
    p.end();

    cv::Mat constraintsMat = ImageConverter::toMatRectifyAlpha(constraintsImg);
    this->mask = ImageConverter::toMatRectifyAlpha(maskImg);

    TangentMap tangents = TangentMap(constraintsMat);
    cv::Mat maskCorners = tangents.getNocorners();

    // Create and initialize crossfield
    this->crossfield = new CrossField(constraintsMat.rows, constraintsMat.cols);
    this->crossfield->initialiceThita(&tangents);
    this->crossfield->setConstraintsMap(maskCorners,constraintsMat);


    for(int i=0; i<this->crossfield->height(); i++) {
        for(int j=0; j<this->crossfield->width(); j++) {
            if(curvatureImg.pixel(j,i) != Qt::transparent)
                this->crossfield->markAsCurvatureLine(i,j);
        }
    }
    this->crossfield->updateConstraintsWithUserInput();

    // Distance Transform
    DistanceTransform dt;
    LabeledMap * labeledMap = dt.filterDiscontinuity(maskCorners, this->crossfield);
    labeledMap->generateInverseMap();

    this->crossfield->initialice(labeledMap,constraintsMat, &tangents);

    // Create Period-jump field
    UnknownsIndexer index_harmonic = UnknownsIndexer(this->mask,2, this->crossfield);
    this->pjumpfield = new PeriodJumpField(constraintsMat.rows,constraintsMat.cols);
    this->pjumpfield->initialice(labeledMap, this->mask, &index_harmonic);
}

void WorkerThread::harmonicSmoothing() {
    // Find topology - Harmonic Smoothing
    UnknownsIndexer index_harmonic = UnknownsIndexer(this->mask,2, this->crossfield); // Never modified...

    HarmonicCrossField smootherStitching(this->crossfield,this->pjumpfield, &index_harmonic);
    smootherStitching.smoothWithIterativeGreedy(); // TODO put callback in there.
}

void WorkerThread::covariantSmoothing() {
    // Index unknowns
    UnknownsIndexer index_covariant(this->mask,4,this->crossfield); // Never changes

    BendField smootherCovariant(this->crossfield,this->pjumpfield, &index_covariant);
    smootherCovariant.smoothBendField();
}

void WorkerThread::rotateCrosses() {
    UnknownsIndexer index_harmonic = UnknownsIndexer(this->mask,2, this->crossfield);
    this->crossfield->rotateCrosses(this->pjumpfield, this->mask, &index_harmonic);
    this->crossfield->checkPositiveAngles(&index_harmonic);
}

QImage WorkerThread::drawNormals(CrossField *crossfield) {
    CrossField3D cf3(crossfield);
    NormalField n(&cf3);
    return n.toImage();
}

QImage WorkerThread::drawShading(CrossField *crossfield) {
    CrossField3D cf3(crossfield);
    NormalField n(&cf3);
    return n.toShadedImage();
}


QImage WorkerThread::drawCrosses(CrossField *crossfield) {
    // The simpler of the two crossfield visualizations in crossfieldgraphic.cpp

    int height = crossfield->height();
    int width = crossfield->width();
    int step = 50;

    QImage img(width, height, QImage::Format_RGBA8888);
    img.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&img);
    QPen pen(QColor(0,0,0), 3);
    painter.setPen(pen);

    for(int i=0; i<height; i += step) { // Note: in my implementation, this is the crossfield's image size
        for(int j=0; j<width; j += step) {
            if(true) { // Not masked
                if(!crossfield->isBoundaryPixel(i,j)) {
                    int x = j;
                    int y = i;
                    Vec2d v_0 = crossfield->getV0(y,x);
                    Vec2d v_1 = crossfield->getV1(y,x);
                    Vec2d v_2 = crossfield->getV2(y,x);
                    Vec2d v_3 = crossfield->getV3(y,x);

                    painter.drawLine(x,y, x+v_0(0)*(step/2), y+v_0(1)*(step/2));
                    painter.drawLine(x,y, x+v_1(0)*(step/2), y+v_1(1)*(step/2));
                    painter.drawLine(x,y, x+v_2(0)*(step/2), y+v_2(1)*(step/2));
                    painter.drawLine(x,y, x+v_3(0)*(step/2), y+v_3(1)*(step/2));
                }
            }
        }
    } // End for loops
    painter.end();

    return img;
}
