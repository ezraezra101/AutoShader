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

#include "crossfield.h"

#define  THRESHOLDSTROKE 0.95

CrossField::CrossField() {
    Q_ASSERT(0=="Never call the default constructor - bad things may happen on destruction");
}

CrossField::~CrossField()
{
    for(int i = 0; i < this->h; i++)
    {
        delete field[i];
        delete alpha[i];
        delete beta[i];
        delete v0_Field[i];
        delete v1_Field[i];
        delete curvatureLines[i];
        delete constraintFliped[i];
        delete strokeWeight[i];
        delete boundaries[i];
        delete constrainedLines[i];
    }
    delete field;
    delete alpha;
    delete beta;
    delete v0_Field;
    delete v1_Field;
    delete curvatureLines;
    delete constraintFliped;
    delete strokeWeight;
    delete boundaries;
    delete constrainedLines;
}

// Create all arrays needed to store the crossfield
void CrossField::createField(int height, int width)
{
    this->h = height;
    this->w = width;

    // Declare the crossfield matrix
    this->field = new double*[this->h];
    this->alpha = new double*[this->h];
    this->beta = new double*[this->h];
    this->v0_Field = new Vec2d*[this->h];
    this->v1_Field = new Vec2d*[this->h];
    this->curvatureLines  = new bool*[this->h];
    this->constraintFliped = new bool*[this->h];
    this->strokeWeight = new double*[this->h];
    this->boundaries = new bool*[this->h];
    this->constrainedLines = new bool*[this->h];


    for(int i = 0; i < this->h; i++)
    {
        field[i] = new double[this->w];
        alpha[i] = new double[this->w];
        beta[i] = new double[this->w];
        this->v0_Field[i] = new Vec2d[this->w];
        this->v1_Field[i] = new Vec2d[this->w];
        this->curvatureLines[i] = new bool[this->w];
        this->constraintFliped[i] = new bool[this->w];
        this->strokeWeight[i] = new double[this->w];
        this->boundaries[i] = new bool[this->w];
        this->constrainedLines[i] = new bool[this->w];

        // Fill it
        for(int j = 0; j < this->w; j++)
        {
            // Θ[i][j]=0
            field[i][j] = 0;
            alpha[i][j] = 0;
            beta[i][j] = 0;
            this->v0_Field[i][j] = Vec2d(0,0);
            this->v1_Field[i][j] = Vec2d(0,0);
            this->curvatureLines[i][j] = false;
            this->constraintFliped[i][j] = false;
            this->strokeWeight[i][j] = 0;
            this->boundaries[i][j] = false;
            this->constrainedLines[i][j] = false;
        }
    }
}

CrossField::CrossField(int height, int width)
{
    this->createField(height,width);
}

// Getters and Setters for both representations
void CrossField::setThita(int i,int j,double thita)
{
    this->field[i][j]=thita;
}

void CrossField::setAlphaBetha(QList<QPoint> pixels,double alpha,double betha)
{

    for(int i = 0;i<pixels.size();i++)
    {
        this->alpha[pixels.at(i).x()][pixels.at(i).y()] = alpha;
        this->beta[pixels.at(i).x()][pixels.at(i).y()] = betha;
    }
}

void CrossField::setAlphaBetha(double alpha,double betha, int i, int j)
{
        this->alpha[i][j] = alpha;
        this->beta[i][j] = betha;
}

void CrossField::setV0V1(double u0, double v0, double u1, double v1, int i, int j)
{
    this->v0_Field[i][j] = Vec2d(u0,v0);
    this->v1_Field[i][j] = Vec2d(u1,v1);
}


void CrossField::setV0(double u0, double v0, int i, int j)
{
    this->v0_Field[i][j] = Vec2d(u0,v0);
}

void CrossField::setV1(double u1, double v1,  int i, int j)
{
    this->v1_Field[i][j] = Vec2d(u1,v1);
}

// For normalizing vector representation
void CrossField::normalize()
{
    for(int i = 2; i < this->h-2; i++)
    {
        for(int j = 2; j < this->w-2; j++)
        {
            this->v0_Field[i][j] = Vec2dOperations::getNormalized(this->v0_Field[i][j]);
            this->v1_Field[i][j] = Vec2dOperations::getNormalized(this->v1_Field[i][j]);
        }
    }
}

void CrossField::initialiceThita( TangentMap * tangents )
{
    for(int i = 0; i < this->h; i++)
    {
        for(int j = 0; j < this->w; j++)
        {
            // Obtain the tangent at centroid
            Vec2d tan = tangents->getTangent(i,j);

            // Obtain th angle thita (with respect to the horizontal)
            double thita = Vec2dOperations::angle(tan);

            // Init constraints
            this->field[i][j] = thita;
        }
    }
}

// Clean thita field
void CrossField::cleanThita()
{
    for(int i = 0; i < this->h; i++)
    {
        for(int j = 0; j < this->w; j++)
        {
            // Init constraints
            this->field[i][j] = 0;
        }
    }
}

// Init the alpha/beta field, given a single angle (called thita)
// As parameters, it takes the tangent field from the image (directions for each pixel)
// and a labeled map, which consists in a distance transform segmentation of the image.
// This algorithm is design to asign a direction for every cell in the crossfield,
// included those corresponding to white pixels in the image. The segmentation provides
// a valid centroid (with a direction) for every region. This method propagates this
// direction to all the white pixels in the same region.
void CrossField::initialice(LabeledMap * labeledmap, Mat image,TangentMap * tangents )
{
    this->cleanThita();

    // Propagate constraints (tangents at each centroid in the labeledMap)
    for(int i = 0; i < this->h; i++)
    {
        for(int j = 0; j < this->w; j++)
        {
            // Obtain the centroid of the cell in which (i,j) is included
            QPoint centroid = labeledmap->getLabel(i,j);

            // If has value
            if(centroid!=QPoint(-1,-1))
            {
                // Obtain the tangent at centroid
                Vec2d tan = tangents->getTangent(centroid.x(),centroid.y());

                // Obtain the angle thita (with respect to the horizontal)
                double thita = Vec2dOperations::angle(tan);

                // Some times, the tangent in the centroid is 0 (due to the blurred version used for that)
                // In this case, we set thita to 0 (to avoid nan response)
                if((tan(0)==0)&&(tan(1)==0))
                {
                    // check if some neighbourd have a good value for it
                    Vec2d tanN1 = tangents->getTangent(centroid.x()+1,centroid.y());
                    Vec2d tanN2 = tangents->getTangent(centroid.x(),centroid.y()+1);
                    Vec2d tanN3 = tangents->getTangent(centroid.x()-1,centroid.y());
                    Vec2d tanN4 = tangents->getTangent(centroid.x(),centroid.y()-1);

                    thita = Vec2dOperations::angle(tanN1);

                    if((tanN1(0)==0)&&(tanN1(1)==0))
                    {
                        thita = Vec2dOperations::angle(tanN2);

                        if((tanN2(0)==0)&&(tanN2(1)==0))
                        {
                            thita = Vec2dOperations::angle(tanN3);

                            if((tanN3(0)==0)&&(tanN3(1)==0))
                            {
                                thita = Vec2dOperations::angle(tanN4);

                                if((tanN4(0)==0)&&(tanN4(1)==0))
                                {
                                    thita = 0;                                  
                                }
                            }
                        }
                    }
                }

                // Set it to the crossfield
                this->field[i][j] = thita;
                this->alpha[i][j] = thita;
                this->beta[i][j] = 0;
            }
        }
    }    

    // Generate the vector representation from alpha beta
    this->generate_V0V1_field();
}


void CrossField::markAsCurvatureLine(int i, int j)
{
    // If is stroke constrained
    if(this->boundaries[i][j] == true)
    {
        this->curvatureLines[i][j] = true;
    }
}

void CrossField::updateConstraintsWithUserInput()
{
    for(int i = 2; i < this->h-2; i++)
    {
        for(int j = 2; j < this->w-2; j++)
        {
          if(noCorners.at<double>(i,j) < THRESHOLDSTROKE)
          {
              this->constrainedLines[i][j] = this->curvatureLines[i][j];
          }
        }
    }
}

// Creates the bool map to know where valid strokes (here called boundaries) are.
// It also computes a weight giving the stroke intensity.
// This map is not the final one, used for imposing the constraints on the optimization.
// The user will select strokes to be "constrained directions" by calling  updateConstraintsWithUserInput()
void CrossField::setConstraintsMap(Mat noCornersImage, Mat sketch)
{
    // For debugging:
    Mat weightMapStroke = Mat::zeros(h,w, CV_8UC3);
    Mat boundariesMapStroke = Mat::zeros(h,w, CV_8UC3);

    // Preprocess input
    Mat grayscaleImage,sketch_double;
    cvtColor(sketch, grayscaleImage, CV_BGR2GRAY);
    cv::normalize(grayscaleImage,sketch_double,0,1,NORM_MINMAX,CV_64FC1);

    // Save no corners
    this->noCorners = noCornersImage;

    for(int i = 2; i < this->h-2; i++)
    {
        for(int j = 2; j < this->w-2; j++)
        {
            // If is boundary
            if(sketch_double.at<double>(i,j) < THRESHOLDSTROKE)
            {
                this->boundaries[i][j] = true;

                boundariesMapStroke.at<Vec3b>(i,j)[0] = 255;
                boundariesMapStroke.at<Vec3b>(i,j)[1] = 0;
                boundariesMapStroke.at<Vec3b>(i,j)[2] = 0;
            }

            // 0 == black -- 1 == white
            // Compute weights
            this->strokeWeight[i][j] = ((1 - noCornersImage.at<double>(i,j))-(1-THRESHOLDSTROKE))/(THRESHOLDSTROKE);

            if(this->strokeWeight[i][j] < 0)
            {
                this->strokeWeight[i][j] = 0;
            }

            weightMapStroke.at<Vec3b>(i,j)[0] = 0;
            weightMapStroke.at<Vec3b>(i,j)[1] = (strokeWeight[i][j]*255);
            weightMapStroke.at<Vec3b>(i,j)[2] = 0;
        }
    }

    imwrite("output/strokeWeight.png",weightMapStroke);
    imwrite("output/noCorners.png",noCornersImage);
    imwrite("output/boundaries.png",boundariesMapStroke);
}

// For debugging
void CrossField::saveInitMap(QString nameAlphaBetha)
{
    // The output
    Mat output2 = Mat::zeros(h,w, CV_8UC3);
    Mat output3 = Mat::zeros(h,w, CV_8UC3);
    Mat output4 = Mat::zeros(h,w, CV_8UC3);
    Mat coloredFieldHSV2 = Mat::zeros(h,w, CV_8UC3);
    Mat coloredFieldHSV3 = Mat::zeros(h,w, CV_8UC3);
    Mat coloredFieldHSV4 = Mat::zeros(h,w, CV_8UC3);

    for(int i = 2; i < this->h-2; i++)
    {
        for(int j = 2; j < this->w-2; j++)
        {

            if((this->alpha[i][j]+this->beta[i][j] != 0))
            {
                int angle = ((alpha[i][j]+beta[i][j]) * 180) / M_PI;
                output2.at<Vec3b>(i,j)[0] = angle /2;
                output2.at<Vec3b>(i,j)[1] = 255;
                output2.at<Vec3b>(i,j)[2] = 255;
            }
            else
            {
                output2.at<Vec3b>(i,j)[0] = 0;
                output2.at<Vec3b>(i,j)[1] = 0;
                output2.at<Vec3b>(i,j)[2] = 0;
            }


            if(this->beta[i][j] != 0)
            {
                int angle = (beta[i][j] * 180) / M_PI;
                output3.at<Vec3b>(i,j)[0] = angle /2;
                output3.at<Vec3b>(i,j)[1] = 255;
                output3.at<Vec3b>(i,j)[2] = 255;
            }
            else
            {
                output3.at<Vec3b>(i,j)[0] = 0;
                output3.at<Vec3b>(i,j)[1] = 0;
                output3.at<Vec3b>(i,j)[2] = 0;
            }

            if(this->field[i][j] != 0)
            {
               int angle = (field[i][j] * 180) / M_PI;
                output4.at<Vec3b>(i,j)[0] = angle /2;
                output4.at<Vec3b>(i,j)[1] = 255;
                output4.at<Vec3b>(i,j)[2] = 255;
            }
            else
            {
                output4.at<Vec3b>(i,j)[0] = 0;
                output4.at<Vec3b>(i,j)[1] = 0;
                output4.at<Vec3b>(i,j)[2] = 0;
            }

        }
    }
    // convert BRG to HSV
    cvtColor(output2,coloredFieldHSV2,CV_HSV2BGR);
    cvtColor(output3,coloredFieldHSV3,CV_HSV2BGR);
    cvtColor(output4,coloredFieldHSV4,CV_HSV2BGR);

    nameAlphaBetha = "images/output/"+ nameAlphaBetha;

    imwrite("output/thitaMap.png",coloredFieldHSV4);
    imwrite(nameAlphaBetha.toStdString(),coloredFieldHSV2);
    imwrite("output/bethaMap.png",coloredFieldHSV3);
}

// Generate the (v0,v1) field from (alpha,beta)
void CrossField::generate_V0V1_field()
{
    for(int i = 2; i < this->h-2; i++)
    {
        for(int j = 2; j < this->w-2; j++)
        {
            double v_0 = alpha[i][j] + beta[i][j];
            double v_1 = alpha[i][j] + (M_PI/2) - beta[i][j];

            this->v0_Field[i][j] = Vec2d(cos(v_0),sin(v_0));
            this->v1_Field[i][j] = Vec2d(cos(v_1),sin(v_1));
        }
    }
}


// Export imformation into a XML file
void CrossField::saveToXML(QString path)
{
    // The file to save
    QDomDocument * doc = new QDomDocument("CrossField");

    // Root of the xml tree
    QDomElement root = doc->createElement("crossfield");

    // Attribute of the root
    root.setAttribute("Width",this->w);
    root.setAttribute("Height",this->h);

    // Save the first element
    doc->appendChild(root);

    for(int i = 0; i < this->h; i++)
    {
        for(int j = 0; j < this->w; j++)
        {
            // Add alpha, beta
            QDomElement crosshair = doc->createElement("cross-hair");

            // i - j
            crosshair.setAttribute("i",i);
            crosshair.setAttribute("j",j);

            //v0-v1
            crosshair.setAttribute("v0u",this->v0_Field[i][j](0));
            crosshair.setAttribute("v0v",this->v0_Field[i][j](1));
            crosshair.setAttribute("v1u",this->v1_Field[i][j](0));
            crosshair.setAttribute("v1v",this->v1_Field[i][j](1));

            // constraints
            if(this->curvatureLines[i][j]==true)
            {
                crosshair.setAttribute("strokeConstrained",1);
            }
            else
            {
                crosshair.setAttribute("strokeConstrained",0);
            }

            // constraints
            if(this->boundaries[i][j]==true)
            {
                crosshair.setAttribute("boundaryPixel",1);
            }
            else
            {
                crosshair.setAttribute("boundaryPixel",0);
            }

            root.appendChild(crosshair);
        }
    }

    // Save
    QString outFile = path;

    QFile file(outFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
       return;

    QTextStream out(&file);
    out << doc->toString();
    file.close();
}


void CrossField::saveObj(QString path)
{

    // SAVE OBJ
    QString vertices;
    QString faces;

    // Remove 0's from frame
    int sizeW = w-4;
    int sizeH = h-4;

    // Add the vertices
    for(int i = 0; i <= sizeH; i++)
    {
        for(int j = 0; j <= sizeW; j++)
        {
            // x,y,z => j,i,0
            vertices += "v " + QString::number(j) + " " + QString::number(i) + " " + QString::number(0.0) + "\n";
        }
    }

     // Add the faces
    for(int i = 0; i < sizeH; i++)
    {
        for(int j = 0; j < sizeW; j++)
        {
            int v1Index = (i*(sizeW+1) + j)+1;
            int v2Index = (i*(sizeW+1) + (j+1))+1;
            int v3Index = ((i+1)*(sizeW+1) + j)+1;
            int v4Index = ((i+1)*(sizeW+1) + (j+1))+1;

            // First triangle
            faces += "f " + QString::number(v1Index) + " " + QString::number(v2Index) + " " + QString::number(v3Index) + "\n";

            // Second triangle
            faces += "f " + QString::number(v2Index) + " " + QString::number(v4Index) + " " + QString::number(v3Index) + "\n";

        }
    }

    QString objStream = vertices + "\n" + faces;

    // Save
    QString outFile = path + "_mesh.OBJ";

    QFile file(outFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
       return;

    QTextStream out(&file);
    out << objStream;
    file.close();
}


void CrossField::loadFromXML(QString path)
{
    // Create document
    QDomDocument doc("CrossField");

    // Open file
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
     return;
    if (!doc.setContent(&file))
    {
     file.close();
     return;
    }
    file.close();

    // print out the element names of all elements that are direct children
    // of the outermost element.
    QDomElement root = doc.documentElement();

    // Create field
    this->createField(root.attribute("Height").toInt(),root.attribute("Width").toInt());

    QDomNode crosshairNode = root.firstChild();

    while(!crosshairNode.isNull())
    {
        // Read the cross-hair
        QDomElement crosshair = crosshairNode.toElement(); // try to convert the node to an element.

        if(!crosshair.isNull())
        {
            // Read attributes
            // i - j
            int i = crosshair.attribute("i").toInt();
            int j = crosshair.attribute("j").toInt();

            //vo-v1
            double v0u = crosshair.attribute("v0u").toDouble();
            double v0v = crosshair.attribute("v0v").toDouble();
            double v1u = crosshair.attribute("v1u").toDouble();
            double v1v = crosshair.attribute("v1v").toDouble();

            // constraints
            int strokeConstrained = (bool)crosshair.attribute("strokeConstrained").toInt();            
            int boundaryPixel = (bool)crosshair.attribute("boundaryPixel").toInt();

            // Record them into the matrix
            this->setV0V1(v0u,v0v,v1u,v1v,i,j);

            this->curvatureLines[i][j] = (bool)strokeConstrained;

            this->boundaries[i][j] = (bool)boundaryPixel;


        }

        // Read next node
        crosshairNode = crosshairNode.nextSibling();
    }
}

// Flipping cross representation. Normal computation requires 2 vectors with angle < 90
// We test the angle between V1 and V2, and if > 90 we flip one by 180.
void CrossField::rotateCrosses(PeriodJumpField * pjumpfield, Mat mask, UnknownsIndexer * index)
{
    // Scan the vector field
    for(int i = 0; i < this->h; i++)
    {
        for(int j = 0; j < this->w; j++)
        {
            if(index->isUnknown(i,j)==true)
            {
                Vec2d V1 = this->getV1(i,j);
                Vec2d V2 = this->getV2(i,j);

                Vec2d t1NotNorm = this->getV0(i,j);
                Vec2d t2NotNorm = this->getV1(i,j);

                // Compute discriminant to make sure quadratic is solvable
                double discriminant = 4*(t1NotNorm(0)*t2NotNorm(0) + t1NotNorm(1)*t2NotNorm(1));

                if (discriminant < 0) // happens if angle is greater than 90
                {
                    if(this->isCurvatureLine(i,j))
                    {
                       this->constraintFliped[i][j] = true;
                    }

                    // Rotate by one
                    this->setV0V1(V1(0),V1(1),V2(0),V2(1),i,j);

                    // Add one to my period jumps
                    if(index->isUnknown(i,j+1)==true)
                    {
                        pjumpfield->setRightP(i,j,pjumpfield->getRightP(i,j)-1);
                    }
                    if(index->isUnknown(i+1,j)==true)
                    {
                        pjumpfield->setBottomP(i,j,pjumpfield->getBottomP(i,j)-1);
                    }
                    if(index->isUnknown(i,j-1)==true)
                    {
                        pjumpfield->setLeftP(i,j,pjumpfield->getLeftP(i,j)+1);
                    }
                     if(index->isUnknown(i-1,j)==true)
                    {
                        pjumpfield->setUpperP(i,j,pjumpfield->getUpperP(i,j)+1);
                    }
                }
            }
        }
    }
}

// Checks counterclockwise ordering of the vectors in the cross
bool CrossField::checkPositiveAngles(UnknownsIndexer * index)
{
    // Scan the vector field
    for(int i = 0; i < this->h; i++)
    {
        for(int j = 0; j < this->w; j++)
        {
            if(index->isUnknown(i,j)==true)
            {
                Vec2d V0 = this->getV0(i,j);
                Vec2d V1 = this->getV1(i,j);

                // Test angle > 0 (counterclockwise ordering of the Vectors)
                if(Vec2dOperations::angleTwoVectors(V0,V1)<0)
                {
                    qDebug() << "WARNING!! angle < 0 at " << i <<"," <<j;
                    return false;
                }
            }
        }
    }

    qDebug() << " Angle checker passed";
    return true;
}
