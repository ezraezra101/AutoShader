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
    //this->ui->glwidget->setCrossFieldGraphic(&this->crossfieldGraphic);

    // Connect signals
    this->connects();
}

MainWindow::~MainWindow()
{
    delete ui;
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
    connect(this->ui->show_cross_fields, SIGNAL(stateChanged()), this, SLOT(showCrossFields()));
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
    msgBox.setText("This hasn't been implemented yet!\nBut we're using it to create a crossfield!"); // TODO
    msgBox.exec();

    WorkerThread w;
    w.makeCrossField(
                this->ui->glwidget->getCanvas(GLWidget::CONSTRAINT_CANVAS).getImage(),
                this->ui->glwidget->getCanvas(GLWidget::CURVATURE_CANVAS).getImage(),
                this->ui->glwidget->getCanvas(GLWidget::MASK_CANVAS).getImage(),
                // Strip this out later:
                this->ui->glwidget
                );
}

void MainWindow::showCrossFields() {
    this->ui->glwidget->showCrossFields(this->ui->show_cross_fields->isChecked());
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
        this->ui->glwidget->setActiveCanvas(c);
        this->ui->glwidget->repaint();
    }
}
