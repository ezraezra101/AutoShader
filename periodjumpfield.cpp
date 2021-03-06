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

#include "periodjumpfield.h"

#define INIT_P_VALUE -60

PeriodJumpField::PeriodJumpField(){}

PeriodJumpField::~PeriodJumpField(){}

// Init the field arrays
void PeriodJumpField::createField(int height,int width)
{
    this->h = height;
    this->w = width;

    // Declare the p-field matrix
    this->field = new int**[this->h];

    for(int i = 0; i < this->h; i++)
    {
        field[i] = new int*[this->w];

        for(int j = 0; j < this->w; j++)
        {
            // The 2 p neigbours (right,bottom)
            field[i][j] = new int[2];

            // Fill it
            for(int k = 0; k < 2; k++)
            {
                field[i][j][k] = INIT_P_VALUE;
            }
        }
    }

    notDefinedP = new QVector<QPair<QPoint,QPoint> >;
}

PeriodJumpField::PeriodJumpField(int height,int width)
{
   this->createField(height,width);
}

// Getters and Setters
int PeriodJumpField::getUpperP(int i, int j)
{
    return this->field[i-1][j][1];
}

int PeriodJumpField::getBottomP(int i, int j)
{
    return this->field[i][j][1];
}

int PeriodJumpField::getLeftP(int i, int j)
{
    return this->field[i][j-1][0];
}

int PeriodJumpField::getRightP(int i, int j)
{
    return this->field[i][j][0];
}

void PeriodJumpField::setP(int i,int j,int index, int p)
{
    if(index == 0)
    {
        this->setRightP(i,j,p);
    }
    else if(index == 1)
    {
        this->setBottomP(i,j,p);
    }
    else if(index == 2)
    {
        this->setLeftP(i,j,p);
    }
    else if(index == 3)
    {
        this->setUpperP(i,j,p);
    }

}

void PeriodJumpField::setP(QPoint i,QPoint j, int p)
{
    // If they are in the same row,
    if(i.x()==j.x())
    {
        // is the right neigbour
        this->setRightP(i.x(),i.y(),p);
    }
    else
    {
        // is the bottom neigbour
        this->setBottomP(i.x(),i.y(),p);
    }
}

void PeriodJumpField::setUpperP(int i, int j, int p)
{
    this->field[i-1][j][1] = p;
}

void PeriodJumpField::setBottomP(int i, int j, int p)
{
    this->field[i][j][1] = p;
}

void PeriodJumpField::setLeftP(int i, int j, int p)
{
    this->field[i][j-1][0] = p;
}

void PeriodJumpField::setRightP(int i, int j, int p)
{
    this->field[i][j][0] = p;
}

// Init the field. It takes as input a labeled map, which consists in a distance transform
// segmentation of the image.
// This algorithm is design to asign p=0 for every cell in p which is not in the border
// between to regions of the labeled map. Border cells are queued in notDefinedP vector,
// to be solved during the greedy mixed-integer optimization (see paper for details)
void PeriodJumpField::initialice(LabeledMap * labeledmap, Mat mask, UnknownsIndexer * index)
{
    // Propagate p using the label map
    // Same region neighbourds have p=0
    for(int i = 2; i < this->h-2; i++)
    {
        for(int j = 2; j < this->w-2; j++)
        {
            if(index->isUnknown(i,j)==true)
            {
                // The labels
                QPoint pixelLabel = labeledmap->getLabel(i,j);
                QPoint pixelRightLabel = labeledmap->getLabel(i,j+1);

                // Check if the right neighbourd has the same label
                // Only if not on the right edge
                if(index->isUnknown(i,j+1)==true)
                {
                    if(pixelLabel == pixelRightLabel)
                    {
                        // If they do, then P is 0
                        field[i][j][0] = 0;
                    }
                    else
                    {
                        // If not, then P is undefined, add it to the list
                        field[i][j][0] = INIT_P_VALUE;
                        this->notDefinedP->push_back(QPair<QPoint,QPoint>(QPoint(i,j),QPoint(i,j+1)));
                    }
                }
                // On borders, p=0
                else if(index->isUnknown(i,j+1)==false)
                {
                    // If they do, then P is 0
                    field[i][j][0] = 0;
                }

                // Same for the bottom neighbourd
                QPoint pixelBottomLabel = labeledmap->getLabel((i+1),j);

                //Check if the bottom neighbourd has the same label
                // Only if not on the bottom edge
                if(index->isUnknown(i+1,j)==true)
                {
                    if(pixelLabel == pixelBottomLabel)
                    {
                        // If they do, then P is 0
                        field[i][j][1] = 0;
                    }
                    else
                    {
                        // If not, then P is undefined, add it to the list
                        field[i][j][1] = INIT_P_VALUE;
                        this->notDefinedP->push_back(QPair<QPoint,QPoint>(QPoint(i,j),QPoint(i+1,j)));

                    }
                }
                // On borders, p=0
                else if(index->isUnknown(i+1,j)==false)
                {
                    // If they do, then P is 0
                    field[i][j][1] = 0;
                }
            }

        }
    }

    qDebug() << "TOSTITCH SIZE " << notDefinedP->size();

    saveField(mask);
}

QVector<QPair<QPoint,QPoint> > * PeriodJumpField::getNonDefinedPList()
{
    return new QVector<QPair<QPoint,QPoint> >(*this->notDefinedP);
}

void PeriodJumpField::saveToXML(QString path)
{
    // The file to save
    QDomDocument * doc = new QDomDocument("PeriodJumpField");

    // Root of the xml tree
    QDomElement root = doc->createElement("periodjumpfield");

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
            QDomElement periodjump = doc->createElement("PeriodJump");
            periodjump.setAttribute("i",i);
            periodjump.setAttribute("j",j);
            periodjump.setAttribute("pRight",this->field[i][j][0]);
            periodjump.setAttribute("pBottom",this->field[i][j][1]);
            root.appendChild(periodjump);
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

void PeriodJumpField::loadFromXML(QString path)
{
    // Create document
    QDomDocument doc("PeriodJumpField");

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

    QDomNode periodjumpNode = root.firstChild();

    while(!periodjumpNode.isNull())
    {
        // Read the cross-hair
        QDomElement periodjump = periodjumpNode.toElement(); // try to convert the node to an element.

        if(!periodjump.isNull())
        {
            // Read attributes
            int i = periodjump.attribute("i").toInt();
            int j = periodjump.attribute("j").toInt();
            int pRight = periodjump.attribute("pRight").toInt();
            int pBottom =  periodjump.attribute("pBottom").toInt();

            // Record them into the matrix
            this->setRightP(i,j,pRight);
            this->setBottomP(i,j,pBottom);
        }

        // Read next node
        periodjumpNode = periodjumpNode.nextSibling();
    }

}


void printField(int*** m,int rows, int cols)
{
    QString s1;
    QString s2;

    for(int i = 0; i < rows; i++)
    {
        s1+= QString::number(i) + ") ";
        s2+= QString::number(i) + ") ";

        for(int j = 0; j < cols; j++)
        {

            s1+= QString::number(m[i][j][0]) + " ";
            s2+= QString::number(m[i][j][1]) + " ";

        }
        s1+="\n";
        s2+="\n";
    }

    qDebug() << s1;
    qDebug() << s2;

}


void PeriodJumpField::saveField(Mat mask)
{
    // The output
    Mat outputRight = Mat::zeros(h,w, CV_8UC3);
    Mat outputBottom = Mat::zeros(h,w, CV_8UC3);

    for(int i = 2; i < h-2; i++)
    {
        for(int j = 2; j < w-2; j++)
        {
            // If the mask allows it
            if(mask.at<Vec3b>(i,j)[0]!=0)
            {
                if(this->field[i][j][0]==0)
                {
                    outputRight.at<Vec3b>(i,j)[0] = 0; // b
                    outputRight.at<Vec3b>(i,j)[1] = 0; // g
                    outputRight.at<Vec3b>(i,j)[2] = 255; // r
                }
                else
                {
                    outputRight.at<Vec3b>(i,j)[0] = 255;
                    outputRight.at<Vec3b>(i,j)[1] = 0;
                    outputRight.at<Vec3b>(i,j)[2] = 0;
                }

                if(this->field[i][j][1]==0)
                {
                    outputBottom.at<Vec3b>(i,j)[0] = 0;
                    outputBottom.at<Vec3b>(i,j)[1] = 0;
                    outputBottom.at<Vec3b>(i,j)[2] = 255;
                }
                else
                {
                    outputBottom.at<Vec3b>(i,j)[0] = 255;
                    outputBottom.at<Vec3b>(i,j)[1] = 0;
                    outputBottom.at<Vec3b>(i,j)[2] = 0;

                }
            }
        }
    }

    imwrite("output/periodJumpRight.png",outputRight);
    imwrite("output/periodJumpBottom.png",outputBottom);
}

void PeriodJumpField::printField()
{

    for(int i = 0; i < this->h; i++)
    {
        for(int j = 0; j < this->w; j++)
        {
            qDebug() << this->field[0];
            qDebug() << this->field[1];
        }
    }
}

