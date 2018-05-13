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

}
#include <unistd.h>
void debugShowImage(QImage image, GLWidget *dummy) {
    dummy->getCanvas(GLWidget::SHADING_CANVAS).setImage(image);
    dummy->repaint();
    DEBUG_PRINT("Showing image");
    usleep(1000 * 1000); // Sleep for a second
}

void WorkerThread::makeCrossField(QImage constraints, QImage curvature, QImage mask, GLWidget *dummy)
{
    DEBUG_PRINT("Making crossfield");

    // Existing code expects constraints and curvature to be combined...
    QPainter p(&constraints);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOver); // probably not too important
    p.drawImage(0,0,curvature);
    p.end();

    cv::Mat constraintsMat = ImageConverter::toMatRectifyAlpha(constraints);
    cv::Mat maskMat = ImageConverter::toMatRectifyAlpha(mask);

    DEBUG_PRINT("Converted to qImage");

    TangentMap tangents = TangentMap(constraintsMat);
    cv::Mat maskCorners = tangents.getNocorners();

    DEBUG_PRINT("Created TangentMap");

    // Create and initialize crossfield
    CrossField *crossfield = new CrossField(constraintsMat.rows, constraintsMat.cols);
    (*crossfield).initialiceThita(&tangents);
    (*crossfield).setConstraintsMap(maskCorners,constraintsMat);

    debugShowImage(drawCrosses((*crossfield)), dummy);
    cout << "Initialized (*crossfield)" << "with size: " << crossfield->width() << "," << crossfield->height() << std::endl;

    ///////////////////////////
    // Finished initialization!
    // Dumb curvature line hack:
    for(int i=0; i<crossfield->height(); i++) {
        for(int j=0; j<crossfield->width(); j++) {
            if(curvature.pixel(j,i) != Qt::transparent)
                crossfield->markAsCurvatureLine(i,j);
        }
    }

    ///////////////////////
    // Perform optimization:
    // Note: Uses tangents, maskCorners, crossfield, maskMat,...?

    crossfield->updateConstraintsWithUserInput();

    // Distance Transform
    DistanceTransform dt;
    LabeledMap * labeledMap = dt.filterDiscontinuity(maskCorners, crossfield);
    labeledMap->generateInverseMap();

    crossfield->initialice(labeledMap,constraintsMat, &tangents);

    debugShowImage(drawCrosses((*crossfield)), dummy);
    DEBUG_PRINT("Initialized with distance transformed label map");

    // Index unknowns
    UnknownsIndexer index_harmonic = UnknownsIndexer(maskMat,2, crossfield);

    // Create Period-jump field
    PeriodJumpField pjumpfield = PeriodJumpField(constraintsMat.rows,constraintsMat.cols);
    pjumpfield.initialice(labeledMap, maskMat, &index_harmonic);

    DEBUG_PRINT("Created Period-jump field");

    debugShowImage(drawCrosses((*crossfield)), dummy);
    DEBUG_PRINT("sigh...");

    // Find topology - Harmonic Smoothing
    HarmonicCrossField smootherStitching(crossfield, &pjumpfield, &index_harmonic, maskMat);
    smootherStitching.smoothWithIterativeGreedy();

    //this->computedTopology = true;

    DEBUG_PRINT("Performed Harmonic stitching");

    // Rotate crosses
    crossfield->rotateCrosses(&pjumpfield, maskMat, &index_harmonic);
    crossfield->checkPositiveAngles(&index_harmonic);

    debugShowImage(drawCrosses((*crossfield)), dummy);
    DEBUG_PRINT("Done!");

    debugShowImage(getNormals(crossfield), dummy);
}

QImage WorkerThread::getNormals(CrossField *crossfield) {
    CrossField3D cf3(crossfield);
    NormalField n(&cf3);
    return n.toImage();
}

QImage WorkerThread::drawCrosses(CrossField &crossfield) {
    // The simpler of the two crossfield visualizations in crossfieldgraphic.cpp

    int height = crossfield.height();
    int width = crossfield.width();
    int step = 50;

    QImage img(height, width, QImage::Format_RGBA8888);
    img.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&img);
    QPen pen(QColor(0,0,0), 3);
    painter.setPen(pen);

    for(int i=0; i<height; i += step) { // Note: in my implementation, this is the crossfield's image size
        for(int j=0; j<width; j += step) {
            if(true) { // Not masked
                if(!crossfield.isBoundaryPixel(i,j)) {
                    int x = j;
                    int y = i;
                    Vec2d v_0 = crossfield.getV0(y,x);
                    Vec2d v_1 = crossfield.getV1(y,x);
                    Vec2d v_2 = crossfield.getV2(y,x);
                    Vec2d v_3 = crossfield.getV3(y,x);

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
