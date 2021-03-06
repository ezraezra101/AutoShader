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

#ifndef HARMONICCROSSFIELD_H
#define HARMONICCROSSFIELD_H

// Includes from the project
#include "crossfield.h"
#include "periodjumpfield.h"
#include "unknownsindexer.h"

// External libraries / headers (Solvers, IO, Debugging)
#include <QDebug>
#include <QTime>
#include <eigen/Eigen/Eigen>
#include <eigen/Eigen/Dense>
#include <eigen/Eigen/Sparse>
#include <eigen/Eigen/SparseQR>
#include <eigen/Eigen/IterativeLinearSolvers>
#include <eigen/Eigen/Dense>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cmath>
#include "math.h"


using namespace cv;
using namespace std;
using namespace Eigen;

typedef Eigen::Triplet<double> T;
typedef SparseMatrix<double, Eigen::ColMajor> SpMat;

#ifndef STITCHDATA_H
#define STITCHDATA_H
struct StitchData{

    double cost;
    double i;
    double approx_p;
};
#endif

// This class takes a partially-initialized period jump field
// (everywhere is 0 when possible) and a crossfield (with
// some sparse constraints defined). And returns an interpolated
// crossfield with a harmonic energy, and a full filled period
// jump field.
class HarmonicCrossField
{

    // The crossfield to be interpolated from sparse constraints
    CrossField * crossfield;

    // The Period jump field to compare any neighbouring pair of
    // crosses in the crossfield
    PeriodJumpField * pjumpfield;
    // The undefineds remaining in the jumpfield
    QVector<QPair<QPoint,QPoint> > * nonDefinedP;

    // For Eigen Solver (representation)
    vector<T>  * tripletList;
    SpMat A;
    VectorXd b;
    VectorXd x;

    // Index of the unknowns in the system.
    // Since the full input image is not for
    // solving, and index is provided to find
    // the unknowns in the crossfield.
    UnknownsIndexer * index;

    // Size of the crossfield
    int h,w;

    bool finishedLastIteration;
    bool isFirstIteration;

    // Initialization of the arrays for the Eigen solver (X,b)
    void inits();

    // Set up of the harmonic system to solve eq. 5 in the paper
    void setUpSystem();

    // Maps the 2D crossfield to X 1D array for Alpha variables
    int getColInAforAlpha(int,int);

    // Maps the 2D crossfield to X 1D array for Beta variables
    int getColInAforBeta(int,int);

    // Maps the X 1D array to 2D crossfield
    QPoint getPixelInField(int);

    // Best P between 2 neighbour crosses computation (check out smoothness measure in the paper for details)
    int getBestP(double alpha_i,double beta_i,double alpha_j,double beta_j, double & bestCost);

    double estimateCost(double,double,double,double,int);

    // Saves the harmonic solution of the system stored in X
    // to the crossfield representation
    void saveIntoCrossfield();

    // Returns period jump information for a single cross [i,j] (right and bottom P)
    int * getP(PeriodJumpField * ,int, int);

    // Return Alpha angle from the cross [i,j] neighbours (right and bottom)
    double * getAlpha(CrossField * , int, int);

    // Return Beta angle from the cross [i,j] neighbours (right and bottom)
    double * getBeta(CrossField * , int, int);

    double printEnergy(CrossField * crossField,PeriodJumpField * pjumpfield,int it);

    // Implementation of Gauss Seidel method for solving
    bool localGaussSeidel(QVector<int> residualsQueue, QSet<int> elementsInQueue);

    // In Order insertion implementation (for Gauss Seidel method)
    void insertInOrder(QList<StitchData> & list, StitchData item);

    // In Order insertion implementation (for Gauss Seidel method)
    void insertInOrder_integer(QList<int> & list, int i);

public:

    // Constructor
    HarmonicCrossField();

    // Constructor
    // Init all the vectors and prepares the system for solving the harmonic
    // Get's :
    // Crossfield (init with the input constraints)
    // Period Jumps (init with 0 when posible - see Greedy mixed-integer optimization section in paper)
    // Unknown Index (indexing constrained cels in the crosfield)
    HarmonicCrossField(CrossField * ,PeriodJumpField *, UnknownsIndexer *);

    // Iterative stitching method for solving the field
    void smoothWithIterativeGreedy();

    void smoothIteration();

    bool isDone();


};

#endif // HARMONICCROSSFIELD_H
