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

#include "vec3doperations.h"

Vec3dOperations::Vec3dOperations()
{
}

Vec3d Vec3dOperations::getNormalized(Vec3d v)
{
    // The norm ((x^2)+(y^2)+(z^2))^(1/2)
    double n = norm(v);

    // if norm != 0
    if(n != 0)
    {
        // Normalization [x/norm,y/norm]
        return Vec3d(v(0)/n,v(1)/n,v(2)/n);
    }
    else
    {
        return Vec3d(v(0),v(1),v(2));
    }
}

double Vec3dOperations::norm(Vec3d v)
{
    // The norm ((x^2)+(y^2)+(z^2))^(1/2)
    return sqrt(pow(v(0),2) + pow(v(1),2) + pow(v(2),2));
}

Vec3d Vec3dOperations::crossProduct(Vec3d const u,Vec3d const v)
{
    return Vec3d((u(1)*v(2))-(u(2)*v(1)),((u(2)*v(0))-(u(0)*v(2))),(u(0)*v(1))-(u(1)*v(0)));
}

double Vec3dOperations::dotProduct(Vec3d const u,Vec3d const v)
{
    return ((u[0]*v[0])+(u[1]*v[1])+(u[2]*v[2]));
}

double Vec3dOperations::angle(Vec3d u,Vec3d v)
{
    return (dotProduct(u,v)/((norm(u)*norm(v))));
}
