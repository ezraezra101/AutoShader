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

#include "distancetransform.h"

void printMat(Mat m)
{
    QString s;
    for(int i = 0; i < m.rows; i++)
    {
        s+= QString::number(i) + ") ";
        for(int j = 0; j < m.cols; j++)
        {
            if(m.channels()<2)
                s+= QString::number(m.at<ushort>(i,j)) + " ";
            else
                s+= "[" + QString::number(m.at<Vec2b>(i,j)[0]) +":"+ QString::number(m.at<Vec2b>(i,j)[1])+"]";
        }
        s+="\n";
    }

    qDebug() << s;

}


void printMatColors(LabeledMap * map,Mat m)
{
    // The output
    Mat output = Mat::zeros(m.rows,m.cols, CV_8UC3);

    QMap<QPoint,QRgb> colors;

    for(int i = 2; i < m.rows-2; i++)
    {
        for(int j = 2; j < m.cols-2; j++)
        {
            QPoint key(map->getLabel(i,j));
            QRgb color = qRgb(0,0,0);

            if(colors.contains(key))
            {
                color = colors.value(key);
            }
            else
            {
                if(key!=QPoint(-1,-1))
                {
                    color = qRgb(rand()%255,rand()%255,rand()%255);
                    colors.insert(key,color);
                }
                else
                {
                    color = qRgb(255,255,255);
                }

            }
            output.at<Vec3b>(i,j)[0] = qBlue(color);
            output.at<Vec3b>(i,j)[1] = qGreen(color);
            output.at<Vec3b>(i,j)[2] = qRed(color);
        }
    }


    imwrite("output/distanceTransform.png",output);

}

DistanceTransform::DistanceTransform()
{
}



LabeledMap * DistanceTransform::filter(Mat image)
{
    int h = image.rows;

    int w = image.cols;

    // The output
    Mat output = Mat::zeros(h,w, CV_8UC1);

    Mat imageConverted;
    image.convertTo(imageConverted,CV_32FC1);
    Mat thresholdedImage;
    threshold(imageConverted,thresholdedImage,0.95,1,THRESH_BINARY);

    Mat toSave;
    normalize(thresholdedImage, toSave, 0, 255, NORM_MINMAX, CV_8UC3);
    imwrite("output/dtThreshold.png",toSave);

    // Mask
    int mask[3][3] = { {4,3,4, },
                       {3,0,3, },
                       {4,3,4  } };

    //Labeled Map (closest point)
    LabeledMap * labelmap = new LabeledMap();

    // Distances
    Mat dist;
    thresholdedImage.convertTo(dist,CV_16UC1);
    dist = dist * 10000;

    Mat distTemp = dist.clone();

    // Loop over the image
    for(int i = 1; i < h-1; i++)
    {
        for(int j = 1; j < w-1; j++)
        {
            // For each pixel, calculate the horizontal distance to the stroke
            // get current distance
            int dmin = dist.at<ushort>(i,j);

            // Check if dmin is 0 (its part of the stroke)
            if(dmin == 0)
            {
                // The closest point is itself.
                labelmap->addPixel(i,j,i,j,dmin);
            }
            else
            {
                int dcurr = 0;

                // o|o|o
                // o|x|-
                // -|-|-
                // Over the upper half of the mask
                for(int n=-1; n <= 1; n++)
                {
                    dcurr = distTemp.at<ushort>(i-1,j+n)+mask[0][1+n];

                    if(dcurr < dmin)
                    {
                        dmin = dcurr;

                        // save the closest
                        if(distTemp.at<ushort>(i-1,j+n)==0)
                        {
                            labelmap->addPixel(i,j,i-1,j+n,dmin);
                        }
                        else
                        {
                            labelmap->addPixel(i,j,labelmap->getLabel(i-1,j+n).x(),labelmap->getLabel(i-1,j+n).y(),dmin);
                        }
                    }
                }

                // checks the left pixel too (out of previous iteration)
                dcurr = distTemp.at<ushort>(i,j-1)+mask[1][0];

                if( dcurr < dmin )
                {
                    dmin = dcurr;

                    // save the closest
                    if(distTemp.at<ushort>(i,j-1)==0)
                    {
                        labelmap->addPixel(i,j,i,j-1,dmin);
                    }
                    else
                    {
                        labelmap->addPixel(i,j,labelmap->getLabel(i,j-1).x(),labelmap->getLabel(i,j-1).y(),dmin);
                    }
                }

            }

            distTemp.at<ushort>(i,j) = dmin;
        }
    }

    // Update dist
    dist = distTemp;

    // Loop over the image
    for(int i = 2; i <= h-1; i++)
    {
        for(int j = 2; j < w-1; j++)
        {
            // For each pixel, calculate the horizontal distance to
            int dmin = dist.at<ushort>(h-i,w-j);

            // Check if dmin is 0 (its part of the stroke)
            if(dmin == 0)
            {
                // The closest point is itself.
                labelmap->addPixel(h-i,w-j,h-i,w-j,dmin);
            }
            else
            {

                int dcurr = 0;

                // -|-|-
                // -|x|o
                // o|o|o
                // Over the bottom half of the mask
                for(int n=-1; n <= 1; n++)
                {

                    dcurr = distTemp.at<ushort>(h-i+1,w-j+n)+mask[2][1+n];

                    if(dcurr < dmin)
                    {
                        dmin = dcurr;

                        // save the closest
                        if(distTemp.at<ushort>(h-i+1,w-j+n)==0)
                        {
                            labelmap->addPixel(h-i,w-j,h-i+1,w-j+n,dmin);
                        }
                        else
                        {
                            labelmap->addPixel(h-i,w-j,labelmap->getLabel(h-i+1,w-j+n).x(),labelmap->getLabel(h-i+1,w-j+n).y(),dmin);
                        }
                    }
                }

                // checks the right pixel too (out of previous iteration)
                dcurr = distTemp.at<ushort>(h-i,w-j+1)+mask[1][2];

                if( dcurr < dmin )
                {
                    dmin = dcurr;

                    // save the closest
                    if(distTemp.at<ushort>(h-i,w-j+1)==0)
                    {
                        labelmap->addPixel(h-i,w-j,h-i,w-j+1,dmin);
                    }
                    else
                    {
                        labelmap->addPixel(h-i,w-j,labelmap->getLabel(h-i,w-j+1).x(),labelmap->getLabel(h-i,w-j+1).y(),dmin);
                    }
                }
            }

            distTemp.at<ushort>(h-i,w-j) = dmin;
        }
    }

    printMatColors(labelmap,distTemp);

    distTemp.convertTo(output,output.type());

    return labelmap;
}


LabeledMap * DistanceTransform::filterDiscontinuity(Mat image, CrossField * crossfield)
{
    int h = image.rows;

    int w = image.cols;

    // The output
    Mat output = Mat::zeros(h,w, CV_8UC1);

    Mat imageConverted;
    image.convertTo(imageConverted,CV_32FC1);
    Mat thresholdedImage;
    threshold(imageConverted,thresholdedImage,0.95,1,THRESH_BINARY);

    Mat toSave;
    normalize(thresholdedImage, toSave, 0, 255, NORM_MINMAX, CV_8UC3);
    imwrite("output/dtThreshold.png",toSave);

    // Mask
    int mask[3][3] = { {4,3,4, },
                       {3,0,3, },
                       {4,3,4  } };

    //Labeled Map (closest point)
    LabeledMap * labelmap = new LabeledMap();

    // Distances
    Mat dist;
    thresholdedImage.convertTo(dist,CV_16UC1);
    dist = dist * 10000;

    Mat distTemp = dist.clone();

    // 2 passes
    for(int k = 0; k < 2; k++)
    {

    // Loop over the image
    for(int i = 1; i < h-1; i++)
    {
        for(int j = 1; j < w-1; j++)
        {
            // For each pixel, calculate the horizontal distance to the stroke
            // get current distance
            int dmin = dist.at<ushort>(i,j);

            if((dmin==0)&&(!crossfield->isBoundaryPixel(i,j)))
            {
                dmin = 10000;
            }

            // Check if dmin is 0 (its part of the stroke)
            if(crossfield->isCurvatureLine(i,j))
            {
               // The closest point is itself.
               labelmap->addPixel(i,j,i,j,dmin);
            }
            else
            {
                if(crossfield->isBoundaryPixel(i,j))
                {
                    dmin = 10000;

                    int dcurr = 0;

                    for(int n=-1; n <= 1; n++)
                    {
                        dcurr = distTemp.at<ushort>(i-1,j+n)+mask[0][1+n];

                        if(dcurr < dmin)

                        {
                            dmin = dcurr;

                            // save the closest
                            if(crossfield->isCurvatureLine(i-1,j+n))
                            {
                                labelmap->addPixel(i,j,i-1,j+n,dmin);
                            }
                            else
                            {
                                labelmap->addPixel(i,j,labelmap->getLabel(i-1,j+n).x(),labelmap->getLabel(i-1,j+n).y(),dmin);
                            }
                        }
                    }

                    dcurr = distTemp.at<ushort>(i,j-1)+mask[1][0];

                    if( dcurr < dmin )
                    {
                        dmin = dcurr;

                        // save the closest
                        if(crossfield->isCurvatureLine(i,j-1))
                        {
                            labelmap->addPixel(i,j,i,j-1,dmin);
                        }
                        else
                        {
                            labelmap->addPixel(i,j,labelmap->getLabel(i,j-1).x(),labelmap->getLabel(i,j-1).y(),dmin);
                        }
                    }
                }
                else
                {

                    int dcurr = 0;

                    // o|o|o
                    // o|x|-
                    // -|-|-
                    // Over the upper half of the mask
                    for(int n=-1; n <= 1; n++)
                    {
                        if(!((crossfield->isBoundaryPixel(i-1,j+n))&&(!crossfield->isCurvatureLine(i-1,j+n))))
                        {
                           dcurr = distTemp.at<ushort>(i-1,j+n)+mask[0][1+n];

                            if(dcurr < dmin)
                            {
                                dmin = dcurr;

                                // save the closest
                                if(crossfield->isCurvatureLine(i-1,j+n))
                                {
                                    labelmap->addPixel(i,j,i-1,j+n,dmin);
                                }
                                else
                                {
                                    labelmap->addPixel(i,j,labelmap->getLabel(i-1,j+n).x(),labelmap->getLabel(i-1,j+n).y(),dmin);
                                }
                            }
                        }
                    }

                    // checks the left pixel too (out of previous iteration)

                    if(!((crossfield->isBoundaryPixel(i,j-1))&&(!crossfield->isCurvatureLine(i,j-1))))
                    {
                        dcurr = distTemp.at<ushort>(i,j-1)+mask[1][0];

                        if( dcurr < dmin )
                        {
                            dmin = dcurr;

                            // save the closest
                            if(crossfield->isCurvatureLine(i,j-1))
                            {
                                labelmap->addPixel(i,j,i,j-1,dmin);
                            }
                            else
                            {
                                labelmap->addPixel(i,j,labelmap->getLabel(i,j-1).x(),labelmap->getLabel(i,j-1).y(),dmin);
                            }
                        }
                    }
                }

            }

            distTemp.at<ushort>(i,j) = dmin;
        }
    }

    // Update dist
    dist = distTemp;

    // Loop over the image
    for(int i = 2; i <= h-1; i++)
    {
        for(int j = 2; j < w-1; j++)
        {
            // For each pixel, calculate the horizontal distance to
            int dmin = dist.at<ushort>(h-i,w-j);

            if((dmin==0)&&(!crossfield->isBoundaryPixel(h-i,w-j)))
            {
                dmin = 10000;
            }

            // Check if dmin is 0 (its part of the stroke)
            if(crossfield->isCurvatureLine(h-i,w-j))
            {
                // The closest point is itself.
                labelmap->addPixel(h-i,w-j,h-i,w-j,dmin);
            }
            else
            {
                if(crossfield->isBoundaryPixel(h-i,w-j))
                {
                    dmin = 10000;

                    int dcurr = 0;

                    for(int n=-1; n <= 1; n++)
                    {
                        dcurr = distTemp.at<ushort>(h-i+1,w-j+n)+mask[2][1+n];

                        if(dcurr < dmin)
                        {
                            dmin = dcurr;

                            // save the closest
                            if(crossfield->isCurvatureLine(h-i+1,w-j+n))
                            {
                                labelmap->addPixel(h-i,w-j,h-i+1,w-j+n,dmin);
                            }
                            else
                            {
                                labelmap->addPixel(h-i,w-j,labelmap->getLabel(h-i+1,w-j+n).x(),labelmap->getLabel(h-i+1,w-j+n).y(),dmin);
                            }
                        }
                    }

                    dcurr = distTemp.at<ushort>(h-i,w-j+1)+mask[1][2];


                    if( dcurr < dmin )
                    {
                        dmin = dcurr;

                        // save the closest
                        if(crossfield->isCurvatureLine(h-i,w-j+1))
                        {
                            labelmap->addPixel(h-i,w-j,h-i,w-j+1,dmin);
                        }
                        else
                        {
                            labelmap->addPixel(h-i,w-j,labelmap->getLabel(h-i,w-j+1).x(),labelmap->getLabel(h-i,w-j+1).y(),dmin);
                        }
                    }
                }
                else
                {


                    int dcurr = 0;

                    // -|-|-
                    // -|x|o
                    // o|o|o
                    // Over the bottom half of the mask
                    for(int n=-1; n <= 1; n++)
                    {
                        if(!((crossfield->isBoundaryPixel(h-i+1,w-j+n))&&(!crossfield->isCurvatureLine(h-i+1,w-j+n))))
                        {
                            dcurr = distTemp.at<ushort>(h-i+1,w-j+n)+mask[2][1+n];

                            if(dcurr < dmin)
                            {
                                dmin = dcurr;

                                // save the closest
                                if(crossfield->isCurvatureLine(h-i+1,w-j+n))
                                {
                                    labelmap->addPixel(h-i,w-j,h-i+1,w-j+n,dmin);
                                }
                                else
                                {
                                    labelmap->addPixel(h-i,w-j,labelmap->getLabel(h-i+1,w-j+n).x(),labelmap->getLabel(h-i+1,w-j+n).y(),dmin);
                                }
                            }
                        }
                    }

                    // checks the right pixel too (out of previous iteration)

                    if(!((crossfield->isBoundaryPixel(h-i,w-j+1))&&(!crossfield->isCurvatureLine(h-i,w-j+1))))
                    {
                        dcurr = distTemp.at<ushort>(h-i,w-j+1)+mask[1][2];

                        if( dcurr < dmin )
                        {
                            dmin = dcurr;

                            // save the closest
                            if(crossfield->isCurvatureLine(h-i,w-j+1))
                            {
                                labelmap->addPixel(h-i,w-j,h-i,w-j+1,dmin);
                            }
                            else
                            {
                                labelmap->addPixel(h-i,w-j,labelmap->getLabel(h-i,w-j+1).x(),labelmap->getLabel(h-i,w-j+1).y(),dmin);
                            }
                        }
                    }


                }
            }

            distTemp.at<ushort>(h-i,w-j) = dmin;
        }

    }

    // Update dist
    dist = distTemp;
    }

    printMatColors(labelmap,distTemp);

    distTemp.convertTo(output,output.type());

    return labelmap;
}

