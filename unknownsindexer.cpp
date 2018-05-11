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

#include "unknownsindexer.h"

UnknownsIndexer::UnknownsIndexer()
{
}

UnknownsIndexer::UnknownsIndexer(Mat mask, int unk, CrossField * crossfield)
{
    unknownsPerPixel = unk;
    indexedPixels = 0;

    // Init unknown bool table
    this->unknown = new bool*[mask.rows];
    this->index = new int*[mask.rows];

    for(int i = 0; i < mask.rows; i++)
    {
        this->unknown[i] = new bool[mask.cols];
        this->index[i] = new int[mask.cols];

        for(int j = 0; j < mask.cols; j++)
        {
            this->unknown[i][j] = false;
            this->index[i][j] = -1;
        }
    }

    Mat unknownImg = Mat::zeros(mask.rows,mask.cols, CV_8UC3);

    // Index all non-black pixels
     for(int i = 2; i < mask.rows-2; i++)
     {
         for(int j = 2; j < mask.cols-2; j++)
         {
             // If the pixel is inside the mask and is a constr
             if(mask.at<Vec3b>(i,j)[0]!=0)
             {

                 // If it's either smoothed constrained or stroke constrained (i,j but also the 2 neighbors)
                 if((!((crossfield->isBoundaryPixel(i,j))&&(!crossfield->isCurvatureLine(i,j))))
                 &&(!((crossfield->isBoundaryPixel(i,j+1))&&(!crossfield->isCurvatureLine(i,j+1)))
                 ||!((crossfield->isBoundaryPixel(i+1,j))&&(!crossfield->isCurvatureLine(i+1,j)))))
                 {
                     // Insert key-value pair
                     index[i][j] = indexedPixels;
                     reverseIndex.insert(indexedPixels,QPoint(i,j));

                     // Increase counter
                     indexedPixels++;

                     // Save pixel constrained
                     this->unknown[i][j] = true;
                     unknownImg.at<Vec3b>(i,j)[0] = 255;
                     unknownImg.at<Vec3b>(i,j)[1] = 255;
                     unknownImg.at<Vec3b>(i,j)[2] = 255;
                  }

             }
         }
     }
     imwrite("output/unknownImg.png",unknownImg);
}


UnknownsIndexer::UnknownsIndexer(Mat mask, int unk)
{
    unknownsPerPixel = unk;
    indexedPixels = 0;

    // Init unknown bool table
    this->unknown = new bool*[mask.rows];
    this->index = new int*[mask.rows];

    for(int i = 0; i < mask.rows; i++)
    {
        this->unknown[i] = new bool[mask.cols];
        this->index[i] = new int[mask.cols];

        for(int j = 0; j < mask.cols; j++)
        {
            this->unknown[i][j] = false;
            this->index[i][j] = -1;
        }
    }

    Mat unknownImg = Mat::zeros(mask.rows,mask.cols, CV_8UC3);

    // Index all non-black pixels
     for(int i = 2; i < mask.rows-2; i++)
     {
         for(int j = 2; j < mask.cols-2; j++)
         {
             // If the pixel is inside the mask and is a constr
             if(mask.at<Vec3b>(i,j)[0]!=0)
             {
                 // Insert key-value pair
                 index[i][j] = indexedPixels;
                 reverseIndex.insert(indexedPixels,QPoint(i,j));

                 // Increase counter
                 indexedPixels++;

                 // Save pixel constrained
                 this->unknown[i][j] = true;
                 unknownImg.at<Vec3b>(i,j)[0] = 255;
                 unknownImg.at<Vec3b>(i,j)[1] = 255;
                 unknownImg.at<Vec3b>(i,j)[2] = 255;
             }
         }
     }

     imwrite("output/unknownDiffusionImg.png",unknownImg);
}

int UnknownsIndexer::getFromIndex(int i, int j, int var)
{
    return (index[i][j] + (var*indexedPixels));
}

QPoint UnknownsIndexer::getPixelIndexed(int idx)
{
    return reverseIndex.value(idx);
}
