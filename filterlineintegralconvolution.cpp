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

#include "filterlineintegralconvolution.h"

#define STOP Vec2d(-1,0)
#define SBOTTOM Vec2d(1,0)
#define SLEFT Vec2d(0,-1)
#define SRIGHT Vec2d(0,1)

FilterLineIntegralConvolution::FilterLineIntegralConvolution()
{
}

QList<QPair<double,double> > FilterLineIntegralConvolution::findNeighbour(Vec2d ** field,int i, int j, int h, int w, int k)
{
    // The list of line neighbourds (p0,p1,p2..,pi) with i = k
    QList<QPair<double,double> > neighbours;

    // The center of the stream line neighbour. The i,j directions in the image are fliped to match the x,y axis.
    double x = j;
    double y = i;

    // fx, fy are the internal pixel deplacement of the stream line. We start in the midle of a pixel (0.5,0.5)
    double fx = 0.5;
    double fy = 0.5;

    // Push the center into the results (y,x)
    neighbours.push_back(QPair<double,double>(y,x));

    // Add neighbours (positive direction)
    for(int l = 0; l < k; l++)
    {
        // Take the last pixel added (current position)
        QPair<double,double> pi = neighbours.last();

        // Check if we are inside the image bounds still.
        if((pi.first < h)&&(pi.first > 0.0)&&(pi.second < w)&&(pi.second > 0.0))
        {
            // Look for the vector in field(Pi-1)
            Vec2d vpi = field[(int)pi.first][(int)pi.second];

            // Normalized
            Vec2d vpiNorm = Vec2dOperations::getNormalized(vpi);

            // Vx and Vy are the directions of the next movement.
            // We stil need to decide if we do a step in the x axis or in the y axis.
            double vx =vpiNorm[0];
            double vy =vpiNorm[1];

            // Tx represents the relationship between the current position inside the pixel (fx)
            // and the magnitud of the deplacement in x (vx). The same for ty.
            double tx = 0;
            double ty = 0;

            // Check if at least one of them is != 0. If one of them is cero, doesn't mather
            // f will be infinite, and we will choose to move in the other direction.
            if((vx!=0)||(vy!=0))
            {
                // Compute fx and fy. It must be always a positive factor.
                if(vx>=0)
                    tx = (1-fx)/vx;
                else
                    tx = -fx/vx;
                if(vy>=0)
                    ty = (1-fy)/vy;
                else
                    ty = -fy/vy;

                // The smaller one is the direction we must follow.
                // Big vx means we want to move in x direction.
                // Big vy means we want to move in y direction.
                // Small (or 0) vx means an infinite tx, we want to move in the y direction.
                // Small (or 0) vy means an infinite ty, we want to move in the x direction.

                if(tx<ty)
                {
                    // qDebug() << "x step";
                    // Go to the right
                    if(vx>0)
                    {
                       x+=1;
                       fy+=vy*tx;
                       fx=0;
                    }
                    // Go to the left
                    else
                    {
                       x-=1;
                       fy+=vy*tx;
                       fx=1;
                    }
                }
                else
                {
                    // qDebug() << "y step";
                    // Go up
                    if(vy>0)
                    {
                       y+=1;
                       fx+=vx*ty;
                       fy=0;
                    }
                    // Go down
                    else
                    {
                       y-=1;
                       fx+=vx*ty;
                       fy=1;
                    }
                }

                // If we are still inside te image.
                if(((int)y < w)&&((int)y > 0)&&((int)x < h)&&((int)x > 0))
                {
                    if(!neighbours.contains(QPair<double,double>(y,x)))
                    {
                        // Add it to the list
                       neighbours.push_back(QPair<double,double>(y,x));
                    }
                    else
                    {
                        // Stop the cicle (we are in a loop)
                        l = k;
                    }
                }
                else
                {
                    // Stop the cicle (out of image)
                    l = k;
                }
            }
            else
            {
                // Stop the cicle (vx and vy = 0 in that point. NO flow.)
                l = k;
            }
        }
    }

    // Reset variables for moving in th other direction
    fx = 0.5;
    fy = 0.5;
    x = j;
    y = i;

    // Add neighbours (negative direction)
    for(int l = 0; l < k; l++)
    {
        // Take the last pixel added, now on the front of the list (current position)
        QPair<double,double> pi = neighbours.first();

        // Check if we are inside the image bounds still.
        if((pi.first < h)&&(pi.first > 0.0)&&(pi.second < w)&&(pi.second > 0.0))
        {
            // Look for the vector in field(Pi-1)
            Vec2d vpi = field[(int)pi.first][(int)pi.second];

            // Normalized
            Vec2d vpiNorm = Vec2dOperations::getNormalized(vpi);

            // Vx and Vy are the directions of the next movement.
            // We stil need to decide if we do a step in the x axis or in the y axis.
            double vx =vpiNorm[0];
            double vy =vpiNorm[1];

            // Tx represents the relationship between the current position inside the pixel (fx)
            // and the magnitud of the deplacement in x (vx). The same for ty.
            double tx = 0;
            double ty = 0;

            // Check if at least one of them is != 0. If one of them is cero, doesn't mather
            // f will be infinite, and we will choose to move in the other direction.
            if((vx!=0)||(vy!=0))
            {
                // Compute fx and fy. It must be always a positive factor.
                if(vx>=0)
                    tx = (1-fx)/vx;
                else
                    tx = -fx/vx;
                if(vy>=0)
                    ty = (1-fy)/vy;
                else
                    ty = -fy/vy;

                // The smaller one is the direction we must follow.
                // Big vx means we want to move in x direction.
                // Big vy means we want to move in y direction.
                // Small (or 0) vx means an infinite tx, we want to move in the y direction.
                // Small (or 0) vy means an infinite ty, we want to move in the x direction.

                if(tx<ty)
                {
                    //qDebug() << "x step";
                     // Go to the left
                    if(vx>0)
                    {
                       x-=1;
                       fy+=vy*tx;
                       fx=0;
                    }
                    else
                    {
                       // Go to the right
                       x+=1;
                       fy+=vy*tx;
                       fx=1;
                    }
                }
                else
                {
                    //qDebug() << "y step";
                    // Go down
                    if(vy>0)
                    {
                       y-=1;
                       fx+=vx*ty;
                       fy=0;
                    }
                    // Go up
                    else
                    {
                       y+=1;
                       fx+=vx*ty;
                       fy=1;
                    }
                }

                // If we are still inside te image.
                if(((int)y < h)&&((int)y > 0)&&((int)x < w)&&((int)x > 0))
                {
                    if(!neighbours.contains(QPair<double,double>(y,x)))
                    {
                        // Add it to the list
                        neighbours.push_back(QPair<double,double>(y,x));
                    }
                    else
                    {
                        // Stop the cicle (we are in a loop)
                        l = k;
                    }
                }
                else
                {
                     // Stop the cicle (out of image)
                    l = k;
                }
            }
            else
            {
                // Stop the cicle (vx and vy = 0 in that point. NO flow.)
                l = k;
            }
        }
    }

    return neighbours;
}


int FilterLineIntegralConvolution::computeConvolution(Mat noisyImage, QList<QPair<double,double> > kernel)
{

    // Initial color
    double color = 0;

    // Averaging using the kernel
    for(int i = 0; i < kernel.size(); i++)
    {
        // Get coords as ints
        int indexX = (int)kernel.at(i).first;
        int indexY = (int)kernel.at(i).second;

        // Addition
        color += noisyImage.at<uchar>(indexX,indexY);

    }

    double finalColor = color / kernel.size();

    // return average
    return (int)finalColor;

}

int FilterLineIntegralConvolution::computeConvolutionGrey(Mat image, QList<QPair<double,double> > kernel)
{

    // Initial color
    double color = 0;

    // Averaging using the kernel
    for(int i = 0; i < kernel.size(); i++)
    {
        // Get coords as ints
        int indexX = (int)kernel.at(i).first;
        int indexY = (int)kernel.at(i).second;

        // Addition
        color += image.at<Vec3b>(indexX,indexY)[0];

    }

    double finalColor = color / kernel.size();

    // return average
    return (int)finalColor;

}


Mat FilterLineIntegralConvolution::filter(Vec2d ** field, int h, int w, int k)
{
    // The result image
    Mat lic(h,w,CV_8UC1);

    // The noisy image
    Mat noisyImage(h,w,CV_8UC1);

    Mat colors(h,w,CV_8UC3);

    // Create the noise Image
    for(int i = 0; i < h; i++)
    {
        for(int j = 0; j < w; j++)
        {
            noisyImage.at<uchar>(i,j) =  rand() % 256;
        }
    }

    // LIC to the noise
    // For each point in the field
    for(int i = 0; i < h; i++)
    {
        for(int j = 0; j < w; j++)
        {
            // Find its line neighbours
            QList<QPair<double,double> > neighbours = findNeighbour(field,i,j,h,w,k);

            if(neighbours.size() > 1)
            {
                lic.at<uchar>(i,j) = computeConvolution(noisyImage,neighbours);

                // color background

                Vec2d v  = field[i][j];
                 int magnitude = (Vec2dOperations::norm(field[i][j])) * 255;

                 double angle  = Vec2dOperations::angle(v);
                angle = fmod(angle,M_PI);


                colors.at<Vec3b>(i,j)[0] = 255-(int)(((angle*180.))/(M_PI));
                colors.at<Vec3b>(i,j)[1] = magnitude;
                colors.at<Vec3b>(i,j)[2] = 255;


            }
            else
            {
                lic.at<uchar>(i,j) = 255;

                // color background

                colors.at<Vec3b>(i,j)[0] = 0;
                colors.at<Vec3b>(i,j)[1] = 0;
                colors.at<Vec3b>(i,j)[2] = 255;
            }



        }
    }


    Mat colorsToSave(h,w,CV_8UC3);
    cvtColor(colors,colorsToSave,CV_HSV2BGR);

    imwrite("output/licTangentsColors.png",colorsToSave);

    // return
    return lic;
}

Mat FilterLineIntegralConvolution::filterWithImage(Vec2d ** field, int h, int w, int k, Mat image)
{
    // The result image
    Mat lic(h,w,CV_8UC3);

    Mat colors(h,w,CV_8UC3);

    // LIC to the noise
    // For each point in the field
    for(int i = 0; i < h; i++)
    {
        for(int j = 0; j < w; j++)
        {
            // Find its line neighbours
            QList<QPair<double,double> > neighbours = findNeighbour(field,i,j,h,w,k);

            if(neighbours.size() > 1)
            {
                double greyColor = computeConvolutionGrey(image,neighbours);
                lic.at<Vec3b>(i,j)[0] = greyColor;
                lic.at<Vec3b>(i,j)[1] = greyColor;
                lic.at<Vec3b>(i,j)[2] = greyColor;

                // color background

                Vec2d v  = field[i][j];
                 int magnitude = (Vec2dOperations::norm(field[i][j])) * 255;

                 double angle  = Vec2dOperations::angle(v);
                angle = fmod(angle,M_PI);


                colors.at<Vec3b>(i,j)[0] = 255-(int)(((angle*180.))/(M_PI));
                colors.at<Vec3b>(i,j)[1] = magnitude;
                colors.at<Vec3b>(i,j)[2] = 255;


            }
            else
            {
                lic.at<Vec3b>(i,j)[0] = 255;
                lic.at<Vec3b>(i,j)[1] = 255;
                lic.at<Vec3b>(i,j)[2] = 255;

                // color background

                colors.at<Vec3b>(i,j)[0] = 0;
                colors.at<Vec3b>(i,j)[1] = 0;
                colors.at<Vec3b>(i,j)[2] = 255;
            }

        }
    }


    Mat colorsToSave(h,w,CV_8UC3);
    cvtColor(colors,colorsToSave,CV_HSV2BGR);

    imwrite("output/licTangentsColors.png",colorsToSave);

    // return
    return lic;
}
