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

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Resize window
    // get the dimension available on this screen
    QSize availableSize = qApp->desktop()->availableGeometry().size();
    int width = availableSize.width();
    int height = availableSize.height();
    width = 0.4*width;
    height = 0.5*height;
    QSize newSize( width, height );

    // Center
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight,Qt::AlignCenter,newSize,qApp->desktop()->availableGeometry()));


    // Set crossfieldgraphic to the glwidget
    this->ui->glwidget->setCrossFieldGraphic(&this->crossfieldGraphic);

    // Inits
    this->inits();

    // Connect signals
    this->connects();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::inits()
{
    this->crossfield = NULL;
    this->pjumpfield = NULL;
    this->tangents = NULL;
    this->index_harmonic = NULL;
    this->index_covariant = NULL;
    this->computedTopology = false;
}

void MainWindow::connects()
{
    connect(this->ui->load_constraints, SIGNAL(clicked()), this, SLOT(loadConstraints()));
    connect(this->ui->load_curvature, SIGNAL(clicked()), this, SLOT(loadCurvature()));
    connect(this->ui->load_mask, SIGNAL(clicked()), this, SLOT(loadMask()));
    connect(this->ui->load_convexity, SIGNAL(clicked()), this, SLOT(loadConvexity()));

    connect(this->ui->draw_constraints, SIGNAL(clicked()), this, SLOT(drawConstraints()));
    connect(this->ui->draw_curvature, SIGNAL(clicked()), this, SLOT(drawCurvature()));
    connect(this->ui->draw_mask, SIGNAL(clicked()), this, SLOT(drawMask()));
    connect(this->ui->draw_convexity, SIGNAL(clicked()), this, SLOT(drawConvexity()));

    connect(this->ui->export_shading, SIGNAL(clicked()), this, SLOT(exportShading()));
    connect(this->ui->show_cross_fields, SIGNAL(clicked()), this, SLOT(showCrossFields()));
}

void MainWindow::loadConstraints() {
    loadImageButton(GLWidget::CONSTRAINT_CANVAS);
}
void MainWindow::loadCurvature() {
    loadImageButton(GLWidget::CURVATURE_CANVAS);
}
void MainWindow::loadMask() {
    loadImageButton(GLWidget::MASK_CANVAS);
}
void MainWindow::loadConvexity() {
    loadImageButton(GLWidget::CONCAVITY_CANVAS);
}

void MainWindow::drawConstraints() {
    this->ui->glwidget->setActiveCanvas(GLWidget::CONSTRAINT_CANVAS);
}
void MainWindow::drawCurvature() {
    this->ui->glwidget->setActiveCanvas(GLWidget::CURVATURE_CANVAS);
}
void MainWindow::drawMask() {
    this->ui->glwidget->setActiveCanvas(GLWidget::MASK_CANVAS);
}
void MainWindow::drawConvexity() {
    this->ui->glwidget->setActiveCanvas(GLWidget::CONCAVITY_CANVAS);
}

void MainWindow::exportShading() {
    QMessageBox msgBox;
    msgBox.setText("This hasn't been implemented yet!"); // TODO
    msgBox.exec();
}
void MainWindow::showCrossFields() {
    QMessageBox msgBox;
    msgBox.setText("This hasn't been implemented yet!"); // TODO
    msgBox.exec();
}

void MainWindow::about()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("About BendFields");
    QString text;
    text =  "BendFields: Regularized Curvature Fields from Rough Concept Sketches (2015) \n\n";
    text += "This software computes cross fields over sketches using the approach especified in BendFields paper:\nhttp://www-sop.inria.fr/reves/Basilic/2015/IBB15/ \n\n";

    text += "Copyright Emmanuel Iarussi, David Bommes, Adrien Bousseau\n";
    text += "emmanueliarussi@gmail.com";

    msgBox.setText(text);
    msgBox.exec();
}



void MainWindow::loadImageButton(GLWidget::CanvasEnum c) {
    // Open File Dialog
    QString sketchFileName = QFileDialog::getOpenFileName(this, tr("Open File"),"../sketches/",tr("Sketch (*.png)"));

    if(!sketchFileName.isEmpty()) {
        QImage img = QImage(sketchFileName);

        if(c == GLWidget::CONSTRAINT_CANVAS) {
            int width = img.width();
            int height = img.height();
            this->ui->glwidget->getCanvas(GLWidget::CONSTRAINT_CANVAS).init(width, height);
            this->ui->glwidget->getCanvas(GLWidget::CURVATURE_CANVAS).init(width, height);
            this->ui->glwidget->getCanvas(GLWidget::MASK_CANVAS).init(width, height);
            this->ui->glwidget->getCanvas(GLWidget::CONCAVITY_CANVAS).init(width, height);
            this->ui->glwidget->getCanvas(GLWidget::CROSSFIELD_CANVAS).init(width, height);
            this->ui->glwidget->getCanvas(GLWidget::NORMALS_CANVAS).init(width, height);
            this->ui->glwidget->getCanvas(GLWidget::SHADING_CANVAS).init(width, height);
        }
        if(!this->ui->glwidget->getCanvas(c).setImage(img, true)) {
            QMessageBox msgBox;
            msgBox.setText("Image sizes must match the original drawing constraints image.");
            msgBox.exec();
        }

        this->ui->glwidget->repaint();
    }
}


/*

void MainWindow::changeResolution(int s)
{
    this->crossfieldGraphic.setStep(s);
    this->ui->glwidget->repaint();
}
void MainWindow::saveCrossField()
{
    if(this->crossfield!=NULL)
    {
        // Save File Dialog
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),"../crossFields/untitled",tr("CrossFields (*.xml)"));

        QString name_crossfield = fileName + "_crossfield.xml";

        QString name_pjumpfield = fileName + "_pjumpfield.xml";

        // Save XML files
        this->crossfield->saveToXML(name_crossfield);
        this->pjumpfield->saveToXML(name_pjumpfield);
    }
}

// Loads a prerecorded crossfield
void MainWindow::loadFromFileButtonClicked()
{
    // Open File Dialog
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"../crossFields/",tr("CrossFields (*.xml)"));
    QString name_crossfield = fileName;
    QString name_pjumpfield = fileName.remove(fileName.lastIndexOf("_"),fileName.size()-fileName.lastIndexOf("_")) + "_pjumpfield.xml";

    if(!fileName.isEmpty())
    {
        // Update cursor
        //this->ui->glwidget->updateCursorConstraint();

        // Create crossfield
        this->crossfield = new CrossField();
        this->pjumpfield = new PeriodJumpField();

        // Load into crossfield
        crossfield->loadFromXML(name_crossfield);
        pjumpfield->loadFromXML(name_pjumpfield);

        // Link it with it's graphic representation
        this->crossfieldGraphic.setCrossField(this->crossfield);

        // Open File Dialog
        QString sketchFileName = QFileDialog::getOpenFileName(this, tr("Open File"),"../sketches/",tr("Sketch (*.png)"));

        // Load sketch
        this->sketch = imread(sketchFileName.toStdString());

        // Load Mask
        QString maskFileName = sketchFileName.remove(sketchFileName.lastIndexOf("."),sketchFileName.size()-sketchFileName.lastIndexOf(".")) + "_mask.png";

        // Load sketch
        this->mask = imread(maskFileName.toStdString());        

        // Set them to the graphic representation of the crossfield
        this->crossfieldGraphic.setSketch(this->sketch);
        this->crossfieldGraphic.setMask(this->mask);

        // Refresh
        this->activeCrossesMode();

        // Resize the view
        this->ui->glwidget->updateView();

    }
}

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
