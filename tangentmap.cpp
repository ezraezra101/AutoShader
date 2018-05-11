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

#include "tangentmap.h"

// Smoothing filter parameters
#define K_FILTERING 3
#define S_FILTERING 1

// Other configurations (depending on the sketch)
// #define K_FILTERING 23
// #define S_FILTERING 50

// #define K_FILTERING 15
// #define S_FILTERING 20

// #define K_FILTERING 13
// #define S_FILTERING 2

// Corner removal kernel size
#define KERNEL_SIZE 13

// Other configurations (depending on the sketch)
// #define KERNEL_SIZE 23
// #define KERNEL_SIZE 1

TangentMap::TangentMap()
{
}

TangentMap::~TangentMap()
{
    for(int i = 0; i < this->h; i++)
    {
        delete tangents[i];
    }

    delete tangents;
}

Vec2d TangentMap::getTangent(int i, int j)
{
    return this->tangents[i][j];
}


Mat TangentMap::preprocessForFiltering(Mat input)
{
    Mat grayscaleImage,blurred;

    // Greyscale version of it
    cvtColor(input, grayscaleImage, CV_BGR2GRAY);

    GaussianBlur(grayscaleImage,blurred,Size(3,3),2,2);

    imwrite("output/blurImage.png",blurred);

    return blurred;
}

double ** TangentMap::gaussianKernel(int k, double sigma)
{
    int k_rows = k;
    int k_cols = k;

    double ** gaussk = new double*[k_rows];
    double accum = 0;

    for(int fx = (-k_rows/2); fx <= (k_rows/2); fx++)
    {
        gaussk[fx+(k_rows/2)] = new double[k_cols];

        for(int fy = (-k_cols/2); fy <= (k_cols/2); fy++)
        {

            double w = (double)(exp((double)(-(pow(fx,2)+pow(fy,2))/(2.0*pow(sigma,2)))) / (2.0 * M_PI * pow(sigma,2)));
            accum+=w;
            gaussk[fx+(k_rows/2)][fy+(k_cols/2)] = w;

        }
    }

    // normalize
    for(int i = 0; i < k_rows; i++)
    {
        for(int j = 0; j < k_cols; j++)
        {
            gaussk[i][j] = (double) (gaussk[i][j]/accum);
        }
    }

    return gaussk;
}

Mat TangentMap::findEigenValues(Mat st)
{

    // Eigenvalues matrix (to return)
    Mat eigenValues = Mat::zeros(st.rows,st.cols, CV_64FC2);


    // Compute eigenvalues
    for(int i = 0; i < st.rows; i++)
    {
        for(int j = 0; j < st.cols; j++)
        {
            double E = st.at<Vec4d>(i,j)[0];
            double F = st.at<Vec4d>(i,j)[1];
            double G = st.at<Vec4d>(i,j)[3];

            // λ_1 = ( E + G + sqrt((E-G)^2 + 4*F^2) ) / 2
            double lambda_1 = (double)((E + G + sqrt( pow((E-G),2.0) + 4.0*pow(F,2.0) )) / 2.0);

            // λ_2 = ( E + G - sqrt((E-G)^2 + 4*F^2) ) / 2
            double lambda_2 = (double)((E + G - sqrt( pow((E-G),2.0) + 4.0*pow(F,2.0) )) / 2.0);

            // Copy into mat
            eigenValues.at<Vec2d>(i,j)[0] =  lambda_1;
            eigenValues.at<Vec2d>(i,j)[1] =  lambda_2;
        }
    }

    return eigenValues;
}

Mat TangentMap::lessChangeEigenvector(Mat st, Mat ev)
{
    // Eigenvalues matrix (to return)
    Mat eigenVectors_less = Mat::zeros(st.rows,st.cols, CV_64FC2);

    // Compute eigenvectors
    for(int i = 0; i < st.rows; i++)
    {
        for(int j = 0; j < st.cols; j++)
        {
            // Two different ways of computing it (λ_1-E, -F) or (λ_2-G,F)
            double F = st.at<Vec4d>(i,j)[1];
            double E = st.at<Vec4d>(i,j)[0];
            double G = st.at<Vec4d>(i,j)[3];
            double lambda_1 = ev.at<Vec2d>(i,j)[0];
            double lambda_2 = ev.at<Vec2d>(i,j)[1];


            // Copy into mat
            // review vertical case
            eigenVectors_less.at<Vec2d>(i,j)[0] =  (lambda_1 - E);
            eigenVectors_less.at<Vec2d>(i,j)[1] =  (-F);

        }
    }

    return eigenVectors_less;
}

Mat TangentMap::smoothFiltering(Mat st, int k, double space_s, double range_s)
{
    Mat st_smoothed = Mat::zeros( st.size(), CV_64FC4 );

    Mat st_normalized;

    normalize(st, st_normalized, 0, 1, NORM_MINMAX, CV_64FC4);

    double ** kernel = gaussianKernel(k,space_s);

    // Smooth
    for(int i = 0; i < st.rows; i++)
    {
        for(int j = 0; j < st.cols; j++)
        {
            // Accum for the convolution (for each channel)
            double convolved_value_ch0 = 0;
            double convolved_value_ch1 = 0;
            double convolved_value_ch2 = 0;
            double convolved_value_ch3 = 0;

            double val_c_ch0 = st_normalized.at<Vec4d>(i,j)[0];
            double val_c_ch1 = st_normalized.at<Vec4d>(i,j)[1];
            double val_c_ch2 = st_normalized.at<Vec4d>(i,j)[2];
            double val_c_ch3 = st_normalized.at<Vec4d>(i,j)[3];

            double normalization_w_ch0 =  0;
            double normalization_w_ch1 =  0;
            double normalization_w_ch2 =  0;
            double normalization_w_ch3 =  0;

            // Convolve kernel
            for(int fi = -(k/2); fi <= (k/2); fi++)
            {
                for(int fj = -(k/2); fj <= (k/2); fj++)
                {                  
                    double val_n_ch0 = st.at<Vec4d>(i,j)[0];
                    double val_n_ch1 = st.at<Vec4d>(i,j)[1];
                    double val_n_ch2 = st.at<Vec4d>(i,j)[2];
                    double val_n_ch3 = st.at<Vec4d>(i,j)[3];

                    double range_ch0 = 1;
                    double range_ch1 = 1;
                    double range_ch2 = 1;
                    double range_ch3 = 1;

                    double range_weight_ch0 =  1;
                    double range_weight_ch1 =  1;
                    double range_weight_ch2 =  1;
                    double range_weight_ch3 =  1;


                    double space_weight_ch =  kernel[fi+(k/2)][fj+(k/2)];

                    if(((i+fi>0)&&(i+fi<st.rows))&&((j+fj>0)&&j+fj<st.cols))
                    {
                        val_n_ch0 = st.at<Vec4d>(i+fi,j+fj)[0];
                        val_n_ch1 = st.at<Vec4d>(i+fi,j+fj)[1];
                        val_n_ch2 = st.at<Vec4d>(i+fi,j+fj)[2];
                        val_n_ch3 = st.at<Vec4d>(i+fi,j+fj)[3];

                        range_ch0 = std::abs(val_c_ch0 - st_normalized.at<Vec4d>(i+fi,j+fj)[0]);
                        range_ch1 = std::abs(val_c_ch1 - st_normalized.at<Vec4d>(i+fi,j+fj)[1]);
                        range_ch2 = std::abs(val_c_ch2 - st_normalized.at<Vec4d>(i+fi,j+fj)[2]);
                        range_ch3 = std::abs(val_c_ch3 - st_normalized.at<Vec4d>(i+fi,j+fj)[3]);

                        range_weight_ch0 =  (double)(exp((double)(-pow(range_ch0,2.0)/(2.0*pow(range_s,2)))) / (2.0 * M_PI * pow(range_s,2)));
                        range_weight_ch1 =  (double)(exp((double)(-pow(range_ch1,2.0)/(2.0*pow(range_s,2)))) / (2.0 * M_PI * pow(range_s,2)));
                        range_weight_ch2 =  (double)(exp((double)(-pow(range_ch2,2.0)/(2.0*pow(range_s,2)))) / (2.0 * M_PI * pow(range_s,2)));
                        range_weight_ch3 =  (double)(exp((double)(-pow(range_ch3,2.0)/(2.0*pow(range_s,2)))) / (2.0 * M_PI * pow(range_s,2)));

                    }

                    normalization_w_ch0 += space_weight_ch * range_weight_ch0;
                    normalization_w_ch1 += space_weight_ch * range_weight_ch1;
                    normalization_w_ch2 += space_weight_ch * range_weight_ch2;
                    normalization_w_ch3 += space_weight_ch * range_weight_ch3;

                    convolved_value_ch0 += val_n_ch0 * (space_weight_ch * range_weight_ch0);
                    convolved_value_ch1 += val_n_ch1 * (space_weight_ch * range_weight_ch1);
                    convolved_value_ch2 += val_n_ch2 * (space_weight_ch * range_weight_ch2);
                    convolved_value_ch3 += val_n_ch3 * (space_weight_ch * range_weight_ch3);
                }
            }

            st_smoothed.at<Vec4d>(i,j)[0] = (double) (convolved_value_ch0 / normalization_w_ch0);
            st_smoothed.at<Vec4d>(i,j)[1] = (double) (convolved_value_ch1 / normalization_w_ch1);
            st_smoothed.at<Vec4d>(i,j)[2] = (double) (convolved_value_ch2 / normalization_w_ch2);
            st_smoothed.at<Vec4d>(i,j)[3] = (double) (convolved_value_ch3 / normalization_w_ch3);
        }
    }

    return st_smoothed;
}

// Generate no corners image, by weigthing the input according to the λ_2 magnitude
Mat TangentMap::generateNoCorners(Mat eigenValues, Mat sketch)
{
    // Get only lambda 2
    Mat lambda2, lambda1;
    vector<Mat> channels(2);
    split(eigenValues,channels);
    lambda1 = channels[0];
    lambda2 = channels[1];

    // Different sizes
    Mat element = getStructuringElement( MORPH_ELLIPSE,Size(KERNEL_SIZE,KERNEL_SIZE));

    lambda2.convertTo(lambda2,CV_32F);

    GaussianBlur(lambda2,lambda2,Size(3,3),2);

    dilate(lambda2,lambda2,element);

    Mat ev_normalized;
    normalize(lambda2, ev_normalized, 0, 1, NORM_MINMAX, CV_64FC1);

    int contrast = 4;

    // Contrast
    for(int i = 0; i < sketch.rows; i++)
    {
        for(int j = 0; j < sketch.cols; j++)
        {
            double val = contrast*ev_normalized.at<double>(i,j);

            if(val>1)
            {
                val = 1;
            }
            ev_normalized.at<double>(i,j) = val;
        }
    }


    Mat sketch_normalized;
    normalize(sketch,sketch_normalized,0,1,NORM_MINMAX,CV_64FC1);

    Mat output = Mat::zeros(sketch.rows,sketch.cols,CV_64FC1);

    for(int i = 0; i < sketch.rows; i++)
    {
        for(int j = 0; j < sketch.cols; j++)
        {
            // Screen blend
            output.at<double>(i,j) = 1-((1-sketch_normalized.at<double>(i,j)) * (1-(ev_normalized.at<double>(i,j))));
        }
    }

    Mat toSave;
    normalize(output, toSave, 0, 255, NORM_MINMAX, CV_8UC3);
    imwrite("output/cornersDetected.png",toSave);

    return output;

}

// Builds the angent XY map with gaussian struture tensor
Vec2d ** TangentMap::myStructureTensorXYMap(Mat preprocImage, Mat input)
{
    // Partian derivatives with sobel

    // Generate sobel X
    Mat gradientMapX;

    // Generate sobel Y
    Mat gradientMapY;

    // Gradient map (blurImage, gradientX, 1(x), 0(y), kernelSize(3), scale 1 (nonscale), delta 0 (nondelta)
    Sobel(preprocImage,gradientMapX,CV_64FC1,1,0,3,1,0);

    // Gradient map (blurImage, gradientY, 0(x), 1(y), kernelSize(3), scale 1 (nonscale), delta 0 (nondelta)
    Sobel(preprocImage,gradientMapY,CV_64FC1,0,1,3,1,0);

    imwrite("output/sobelX.png",gradientMapX);
    imwrite("output/sobelY.png",gradientMapY);

    // Normalize
    normalize(gradientMapX, gradientMapX, -1, 1, NORM_INF , CV_64FC1);
    normalize(gradientMapY, gradientMapY, -1, 1, NORM_INF, CV_64FC1);

    // Compute structure tensor. Each component in an image channel
    Mat structureTensorM = Mat::zeros( preprocImage.size(), CV_64FC4 );

    for(int i = 0; i < preprocImage.rows; i++)
    {
        for(int j = 0; j < preprocImage.cols; j++)
        {
            // Compute struture tensor for each pixel
            // df/dx * df/dx
            structureTensorM.at<Vec4d>(i,j)[0] = gradientMapX.at<double>(i,j) * gradientMapX.at<double>(i,j);
            // df/dx * df/dy
            structureTensorM.at<Vec4d>(i,j)[1] = gradientMapX.at<double>(i,j) * gradientMapY.at<double>(i,j);
            // df/dx * df/dy
            structureTensorM.at<Vec4d>(i,j)[2] = gradientMapX.at<double>(i,j) * gradientMapY.at<double>(i,j);
            // df/dy * df/dy
            structureTensorM.at<Vec4d>(i,j)[3] = gradientMapY.at<double>(i,j) * gradientMapY.at<double>(i,j);

        }

    }

    //Smoothing
    Mat smoothTensor = smoothFiltering(structureTensorM, K_FILTERING,S_FILTERING,0.5);

    // Find Eigenvalues
    Mat eigenValues = this->findEigenValues(smoothTensor);


    // Find tangent eigenvector
    Mat lessChangeVector = this->lessChangeEigenvector(smoothTensor,eigenValues);

    // Generate no-corners
    noCorners = this->generateNoCorners(eigenValues,preprocImage);

    // Declare the matrix
    Vec2d ** matrix = new Vec2d*[this->h];

    // Fill it
    for(int i = 0; i < this->h; i++)
    {
        matrix[i] = new Vec2d[this->w];

        // Fill it
        for(int j = 0; j < this->w; j++)
        {
            double x_st = lessChangeVector.at<Vec2d>(i,j)[0];
            double y_st = lessChangeVector.at<Vec2d>(i,j)[1];

            // The gradient vector
            Vec2d stVector = Vec2d(x_st,y_st);

            // Normalized
            Vec2d stVectorNormalized = Vec2dOperations::getNormalized(stVector);

            // The ortogonal
            matrix[i][j] = stVectorNormalized;
        }
    }

    // Save LIC of tangent flow
    FilterLineIntegralConvolution lic;
    Mat licTangent = lic.filter(matrix,this->h,this->w,7);
    imwrite("output/licTangents.png",licTangent);

    return matrix;
}


TangentMap::TangentMap(Mat input)
{   
    // Dimensions
    this->w = input.cols;
    this->h = input.rows;

    // Blur + Grayscale
    Mat preprocImage = this->preprocessForFiltering(input);

    // Merge both gradients (x and y) fields
    this->tangents = this->myStructureTensorXYMap(preprocImage,input);
}


TangentMap::TangentMap(QString uFileName, QString vFileName, Mat sketch)
{
    // Open files
    QFile file_u(uFileName);
    if (!file_u.open(QIODevice::ReadOnly))
     return;

    QFile file_v(vFileName);
    if (!file_v.open(QIODevice::ReadOnly))
     return;

    // Read
    QTextStream in_u(&file_u);
    QTextStream in_v(&file_v);

    // Read first line to get size
    QString line = in_u.readLine();
    in_v.readLine();
    QStringList fields = line.split(",");
    this->w = fields.first().toInt();
    this->h = fields.last().toInt();

    // Declare the matrix
    this->tangents = new Vec2d*[this->h];

    // Init it
    for(int i = 0; i < this->h; i++)
    {
        this->tangents[i] = new Vec2d[this->w];

        // Fill it
        for(int j = 0; j < this->w; j++)
        {
            // The gradient vector
            Vec2d stVector = Vec2d(0,0);

            // The ortogonal
            this->tangents[i][j] = stVector;
        }
    }

    // Blur + Grayscale
    Mat preprocImage = this->preprocessForFiltering(sketch);

    Mat outputU = Mat::zeros(h,w, CV_8UC3);

    // Fill it with real data
    while(!in_u.atEnd()) {

        QString line_u = in_u.readLine();
        QStringList fields_u = line_u.split(",");

        int i_u = fields_u.at(0).toInt();
        int j_u = fields_u.at(1).toInt();

        if(sketch.at<Vec3b>(j_u,i_u)[0]==255)
        {
            double tangent_x = fields_u.at(2).toDouble();
            double tangent_y = fields_u.at(3).toDouble();

            // The gradient vector
            Vec2d stVector = Vec2d(tangent_x,tangent_y);

            // Normalized
            Vec2d stVectorNormalized = Vec2dOperations::getNormalized(stVector);

            if(Vec2dOperations::norm(stVector)!=0)
            {
                outputU.at<Vec3b>(j_u,i_u)[0] = 255;
                outputU.at<Vec3b>(j_u,i_u)[1] = 255;
                outputU.at<Vec3b>(j_u,i_u)[2] = 255;

                this->tangents[j_u][i_u] = stVectorNormalized;
            }
        }
    }

    imwrite("output/fieldU.png",outputU);

    Mat outputV = Mat::zeros(h,w, CV_8UC3);

    // Fill it with real data
    while(!in_v.atEnd()) {

        QString line_v = in_v.readLine();
        QStringList fields_v = line_v.split(",");

        int i_v = fields_v.at(0).toInt();
        int j_v = fields_v.at(1).toInt();

        if(sketch.at<Vec3b>(j_v,i_v)[2]==255)
        {
            double tangent_x = fields_v.at(2).toDouble();
            double tangent_y = fields_v.at(3).toDouble();

            // The gradient vector
            Vec2d stVector = Vec2d(tangent_x,tangent_y);

            // Normalized
            Vec2d stVectorNormalized = Vec2dOperations::getNormalized(stVector);

            if(Vec2dOperations::norm(stVector)!=0)
            {
                outputV.at<Vec3b>(j_v,i_v)[0] = 255;
                outputV.at<Vec3b>(j_v,i_v)[1] = 255;
                outputV.at<Vec3b>(j_v,i_v)[2] = 255;

                this->tangents[j_v][i_v] = stVectorNormalized;
            }
        }
    }

    imwrite("output/fieldV.png",outputV);

    // Generate sobel X
    Mat gradientMapX;

    // Generate sobel Y
    Mat gradientMapY;

    // Gradient map (blurImage, gradientX, 1(x), 0(y), kernelSize(3), scale 1 (nonscale), delta 0 (nondelta)
    Sobel(preprocImage,gradientMapX,CV_64FC1,1,0,3,1,0);

    // Gradient map (blurImage, gradientY, 0(x), 1(y), kernelSize(3), scale 1 (nonscale), delta 0 (nondelta)
    Sobel(preprocImage,gradientMapY,CV_64FC1,0,1,3,1,0);

    imwrite("output/sobelX.png",gradientMapX);
    imwrite("output/sobelY.png",gradientMapY);

    // Normalize
    normalize(gradientMapX, gradientMapX, -1, 1, NORM_MINMAX , CV_64FC1);
    normalize(gradientMapY, gradientMapY, -1, 1, NORM_MINMAX, CV_64FC1);

    // Compute structure tensor. Each component in an image channel
    Mat structureTensorM = Mat::zeros( preprocImage.size(), CV_64FC4 );

    for(int i = 0; i < preprocImage.rows; i++)
    {
        for(int j = 0; j < preprocImage.cols; j++)
        {
            // Compute struture tensor for each pixel
            // df/dx * df/dx
            structureTensorM.at<Vec4d>(i,j)[0] = gradientMapX.at<double>(i,j) * gradientMapX.at<double>(i,j);
            // df/dx * df/dy
            structureTensorM.at<Vec4d>(i,j)[1] = gradientMapX.at<double>(i,j) * gradientMapY.at<double>(i,j);
            // df/dx * df/dy
            structureTensorM.at<Vec4d>(i,j)[2] = gradientMapX.at<double>(i,j) * gradientMapY.at<double>(i,j);
            // df/dy * df/dy
            structureTensorM.at<Vec4d>(i,j)[3] = gradientMapY.at<double>(i,j) * gradientMapY.at<double>(i,j);

        }

    }

    //Smmothing
    Mat smoothTensor = smoothFiltering(structureTensorM, 3,1,0.5);

    // Find Eigenvalues
    Mat eigenValues = this->findEigenValues(smoothTensor);

    this->noCorners = this->generateNoCorners(eigenValues,preprocImage);

    file_u.close();
    file_v.close();

}
