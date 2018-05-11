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

#ifndef LABELEDMAP_H
#define LABELEDMAP_H

// External libraries / headers (Solvers, IO, Debugging)
#include <QPoint>
#include <QMap>
#include <QMultiMap>
#include <QRgb>
#include <QDebug>


// Overload of < operator for comparing QPoints
// and being able to use them as a QMap Key
inline bool operator<(const QPoint& lhs, const QPoint& rhs)
{
    if (lhs.x() < rhs.x())
        return true;
    else if (lhs.x() == rhs.x())
        return lhs.y() < rhs.y();
    else
        return false;
}


// Class holding the label for each pixel in the image.
// Each label is, in fact a Point, which is the nearest pixel.
class LabeledMap
{
    // Given a pixel, I can get the label of that cell
    QMap<QPoint,QPoint> labelsByPixel;

    // Given a label, I can get the pixels associated with it
    QMultiMap<QPoint,QPoint> pixelsByLabel;


    // Cost (distance to the label in the Distance transform map)
    QMap<QPoint,int> costByPixel;


public:


    LabeledMap();

    // Add a new point to the map
    void addPixel(int,int,int,int,int);

    // Obtain the label for a point
    QPoint getLabel(int,int);

    // Obtain a list of pixels with an especific label
    QList<QPoint> getPixels(QPoint);

    void generateInverseMap();

    bool hasLabel(int,int);

    // Obtain the cost of a pixel in the distance transform map
    int getCost(QPoint);

    // Amount of pixels labeled
    int size() { return pixelsByLabel.uniqueKeys().size(); }


};


#endif // LABELEDMAP_H
