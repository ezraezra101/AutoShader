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
    DEBUG_PRINT("Initialized (*crossfield)");

    ///////////////////////////
    // Finished initialization!
    // Dumb curvature line hack:
    for(int i=0; i<constraintsMat.rows; i++) {
        for(int j=0; j<constraintsMat.cols; j++) {
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


/*
    // Graphics
    Ui::MainWindow *ui;
    CrossFieldGraphic crossfieldGraphic;

    // Images
    Mat mask;
    Mat sketch;
    Mat maskCorners;

    // CrossFields
    CrossField * crossfield;

    // Tangents
    TangentMap * tangents;

    // Period-jump field
    PeriodJumpField * pjumpfield;

    // Unknowns indexers
    UnknownsIndexer * index_harmonic;
    UnknownsIndexer * index_covariant;

    // Computed topology
    bool computedTopology;

    // Open File name
    QString openFileName;
....

// Process files
void MainWindow::loadImageButtonClicked()
{
    // Open File Dialog
    QString sketchFileName = QFileDialog::getOpenFileName(this, tr("Open File"),"../sketches/",tr("Sketch (*.png)"));

    if(!sketchFileName.isEmpty())
    {
        // Update cursor
        //this->ui->glwidget->updateCursorConstraint();

        // File Names
        QString fileMask = sketchFileName;
        QString maskFileName = fileMask.remove(fileMask.lastIndexOf("."),fileMask.size()-fileMask.lastIndexOf(".")) + "_mask.png";

        // Keep the image name
        openFileName = fileMask.remove(0,fileMask.lastIndexOf("/")+1);

        // Read them, genereta tangents
        this->sketch = imread(sketchFileName.toStdString());
        this->tangents = new TangentMap(sketch);
        this->mask = imread(maskFileName.toStdString());

        // Get no corners image
        this->maskCorners = tangents->getNocorners();

        // Set them to the graphic representation of the crossfield
        this->crossfieldGraphic.setSketch(this->sketch);
        this->crossfieldGraphic.setMask(this->mask);
        this->crossfieldGraphic.setCornersMask(this->maskCorners);

        // Create and initialice crossfield
        this->crossfield = new CrossField(this->sketch.rows,this->sketch.cols);
        this->crossfield->initialiceThita(tangents);
        this->crossfield->setConstraintsMap(maskCorners,sketch);

        // Link it with it's graphic representation
        this->crossfieldGraphic.setCrossField(this->crossfield);

        // Resize the view
        this->ui->glwidget->updateView();

        this->activeConstraintMode();
    }
}

// Process files
void MainWindow::processImageButtonClicked()
{
    if(this->crossfield!= NULL)
    {
        // Update cursor
        this->ui->glwidget->updateCursorWait();

        this->crossfield->updateConstraintsWithUserInput();

        // Distance Transform
        DistanceTransform dt;
        LabeledMap * labeledMap = dt.filterDiscontinuity(maskCorners,crossfield);
        labeledMap->generateInverseMap();

        this->crossfield->initialice(labeledMap,this->sketch,tangents);

        // Index uknowns
        this->index_harmonic = new UnknownsIndexer(this->mask,2,this->crossfield);

        // Create Period-jump field
        this->pjumpfield = new PeriodJumpField(this->sketch.rows,this->sketch.cols);
        pjumpfield->initialice(labeledMap, this->mask, index_harmonic);

        this->activeCrossesMode();

        // Find topology - Harmonic Smoothing
        HarmonicCrossField smootherStitching(this->crossfield,this->pjumpfield, index_harmonic, this->mask);
        smootherStitching.smoothWithIterativeGreedy(this->ui->glwidget);

        this->computedTopology = true;

        // Rotate crosses
        this->crossfield->rotateCrosses(pjumpfield, this->mask, index_harmonic);
        this->crossfield->checkPositiveAngles(index_harmonic);

        // Update cursor
        this->ui->glwidget->updateCursorArrow();
    }
}

// Process files
void MainWindow::covariantProcessImageButtonClicked()
{
    if(this->crossfield!= NULL)
    {
        // Update cursor
        this->ui->glwidget->updateCursorWait();

        if(!this->computedTopology)
        {

            this->crossfield->updateConstraintsWithUserInput();

            // Distance Transform
            DistanceTransform dt;
            LabeledMap * labeledMap = dt.filterDiscontinuity(maskCorners,crossfield);

            labeledMap->generateInverseMap();

            this->crossfield->initialice(labeledMap,this->sketch,tangents);

            this->activeCrossesMode();

            // Index uknowns
            this->index_harmonic = new UnknownsIndexer(this->mask,2,this->crossfield);

            // Create Period-jump field
            this->pjumpfield = new PeriodJumpField(this->sketch.rows,this->sketch.cols);
            pjumpfield->initialice(labeledMap, this->mask, index_harmonic);

            // Find topology - Harmonic Smoothing
            HarmonicCrossField smootherStitching(this->crossfield,this->pjumpfield,index_harmonic,this->mask);
            smootherStitching.smoothWithIterativeGreedy(this->ui->glwidget);

            this->computedTopology = true;
        }

        this->activeCrossesMode();

        // Index uknowns
        this->index_covariant = new UnknownsIndexer(this->mask,4,this->crossfield);

        // Covariant Smoothing
        BendField smootherCovariant(this->crossfield,this->pjumpfield, index_covariant, this->mask);
        smootherCovariant.smoothBendField(this->ui->glwidget);

        this->crossfield->rotateCrosses(pjumpfield,this->mask, index_covariant);
        this->crossfield->checkPositiveAngles(index_covariant);

        this->ui->glwidget->repaint();

        // Update cursor
        this->ui->glwidget->updateCursorArrow();
    }
}
*/
