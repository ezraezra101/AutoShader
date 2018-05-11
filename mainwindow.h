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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Includes from the project
#include "crossfieldgraphic.h"
#include "crossfield.h"
#include "harmoniccrossfield.h"
#include "bendfield.h"
#include "periodjumpfield.h"
#include "distancetransform.h"
#include "unknownsindexer.h"

// External libraries / headers (Solvers, IO, Debugging)
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <cv.h>
#include <highgui.h>
#include <QStyle>
#include <QDesktopWidget>

using namespace cv;

namespace Ui {
    class MainWindow;
}

// Main window of the system.
// It handles events and OpenGL window
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

private slots:

    // Load from file
    void loadFromFileButtonClicked();

    // load Image files
    void loadImageButtonClicked();

    // Process Topology Image files
    void processImageButtonClicked();

    // Process covariant
    void covariantProcessImageButtonClicked();

    // Crosses Mode
    void activeCrossesMode();

    // Hatching Mode
    void activeHatchingMode();

    // Constraint Mode
    void activeConstraintMode();

    // Change Resolution
    void changeResolution(int);

    // Save field
    void saveCrossField();

    // Show Dialog ABOUT
    void about();

private:

    // Methods
    void connects();
    void inits();

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

};

#endif // MAINWINDOW_H
