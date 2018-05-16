#include "worker.h"

#include <QMessageBox>


Worker::Worker()
{
    this->crossfield = NULL;
    this->pjumpfield = NULL;
    this->index_harmonic = NULL;
    this->harmoniccrossfield = NULL;
    this->index_covariant = NULL;
    this->bendfield = NULL;
    this->crossfield3d = NULL;
}

Worker::Worker(QImage constraints, QImage curvature, QImage maskImg, QImage concavity)
{
    qDebug() << "Made worker";
    this->crossfield = NULL;
    this->pjumpfield = NULL;
    this->index_harmonic = NULL;
    this->harmoniccrossfield = NULL;
    this->index_covariant = NULL;
    this->bendfield = NULL;
    this->crossfield3d = NULL;
    this->constraints = constraints;
    this->curvature = curvature;
    this->maskImg = maskImg;
    this->concavity = concavity;
}

#define DELETE_IF_NOT_NULL(x) {if((x) != NULL) { delete (x); }}
Worker::~Worker() {
    DELETE_IF_NOT_NULL(this->crossfield);
    DELETE_IF_NOT_NULL(this->pjumpfield);
    DELETE_IF_NOT_NULL(this->index_harmonic);
    DELETE_IF_NOT_NULL(this->harmoniccrossfield);
    DELETE_IF_NOT_NULL(this->index_covariant);
    DELETE_IF_NOT_NULL(this->bendfield);
    DELETE_IF_NOT_NULL(this->crossfield3d);
}


void Worker::smoothingIteration() {
    QTime time;
    time.start();
    if(this->crossfield == NULL) {
        qDebug() << "Initializing crossfield";
        this->initializeCrossField();

    } else if(this->harmoniccrossfield == NULL) {
        qDebug() << "Initializing harmonic crossfield";
        // Find topology - Harmonic Smoothing
        this->index_harmonic =  new UnknownsIndexer(this->mask,2, this->crossfield);
        this->harmoniccrossfield = new HarmonicCrossField(this->crossfield,this->pjumpfield, this->index_harmonic);
//        HarmonicCrossFieldPristine hcp(this->crossfield, this->pjumpfield, this->index_harmonic, this->mask);
//        hcp.smoothWithIterativeGreedy();

    } else if(!this->harmoniccrossfield->isDone()) {
        qDebug() << "Harmonic smoothing";
        this->harmoniccrossfield->smoothIteration();

    } else if(this->bendfield == NULL) {
        qDebug() << "Initializing bendfield smoother";
        this->index_covariant = new UnknownsIndexer(this->mask,4,this->crossfield);
        this->bendfield = new BendField(this->crossfield,this->pjumpfield, this->index_covariant);

    } else if(!this->bendfield->isDone()) {
        qDebug() << "Bendfield smoothing";
        bendfield->smoothBendField();
    }

    qDebug() << time.elapsed() << "ms spent this smoothing iteration";
}

void Worker::updateConcavity(QImage concavity) {
    this->concavity = concavity;
    rotateCrosses();
    makeCrossField3d(this->concavity);
}

bool Worker::isDone() const {
    return (this->bendfield != NULL) && this->bendfield->isDone();
}

void Worker::initializeCrossField() {
    if(this->pjumpfield != NULL) {
        delete this->pjumpfield;
    }
    if(this->crossfield != NULL) {
        delete this->crossfield;
    }

    // Existing code expects constraints and curvature to be combined...
    QPainter p(&(this->constraints));
    p.setCompositionMode(QPainter::CompositionMode_DestinationOver); // probably not too important
    p.drawImage(0,0,this->curvature);
    p.end();

    cv::Mat constraintsMat = ImageConverter::toMatRectifyAlpha(this->constraints);
    this->mask = ImageConverter::toMatRectifyAlpha(this->maskImg);

    TangentMap tangents = TangentMap(constraintsMat);
    cv::Mat maskCorners = tangents.getNocorners();

    // Create and initialize crossfield
    this->crossfield = new CrossField(constraintsMat.rows, constraintsMat.cols);
    this->crossfield->initialiceThita(&tangents);
    this->crossfield->setConstraintsMap(maskCorners,constraintsMat);


    // Sigh... crossfield (and everything else) is transposed from the QImages.
    for(int i=0; i<this->crossfield->height(); i++) {
        for(int j=0; j<this->crossfield->width(); j++) {
            if(this->curvature.pixel(i,j) != Qt::transparent)
                this->crossfield->markAsCurvatureLine(i,j);
        }
    }
    this->crossfield->updateConstraintsWithUserInput();

    // Distance Transform
    DistanceTransform dt;
    LabeledMap * labeledMap = dt.filterDiscontinuity(maskCorners, this->crossfield);
    labeledMap->generateInverseMap();

    this->crossfield->initialice(labeledMap,constraintsMat, &tangents);
    // Create Period-jump field
    UnknownsIndexer index_harmonic = UnknownsIndexer(this->mask,2, this->crossfield);
    this->pjumpfield = new PeriodJumpField(this->crossfield->height(),this->crossfield->width());
    this->pjumpfield->initialice(labeledMap, this->mask, &index_harmonic);
}
/*
void Worker::initializeCrossField() {
    if(this->pjumpfield != NULL) {
        delete this->pjumpfield;
    }
    if(this->crossfield != NULL) {
        delete this->crossfield;
    }

    // Existing code expects constraints and curvature to be combined...
    QPainter p(&(this->constraints));
    p.setCompositionMode(QPainter::CompositionMode_DestinationOver); // probably not too important
    p.drawImage(0,0,this->curvature);
    p.end();

    cv::Mat constraintsMat = ImageConverter::toMatRectifyAlpha(this->constraints);
    this->mask = ImageConverter::toMatRectifyAlpha(this->maskImg);

    TangentMap tangents = TangentMap(constraintsMat);
    cv::Mat maskCorners = tangents.getNocorners();

    // Create and initialize crossfield
    this->crossfield = new CrossField(constraintsMat.rows, constraintsMat.cols);
    this->crossfield->initialiceThita(&tangents);
    this->crossfield->setConstraintsMap(maskCorners,constraintsMat);


    // Sigh... crossfield (and everything else) is transposed from the QImages.
    for(int i=0; i<this->crossfield->height(); i++) {
        for(int j=0; j<this->crossfield->width(); j++) {
            if(this->curvature.pixel(i,j) != Qt::transparent)
                this->crossfield->markAsCurvatureLine(i,j);
        }
    }
    this->crossfield->updateConstraintsWithUserInput();

    // Distance Transform
    DistanceTransform dt;
    LabeledMap * labeledMap = dt.filterDiscontinuity(maskCorners, this->crossfield);
    labeledMap->generateInverseMap();

    this->crossfield->initialice(labeledMap,constraintsMat, &tangents);
    // Create Period-jump field
    UnknownsIndexer index_harmonic = UnknownsIndexer(this->mask,2, this->crossfield);
    this->pjumpfield = new PeriodJumpField(this->crossfield->height(),this->crossfield->width());
    this->pjumpfield->initialice(labeledMap, this->mask, &index_harmonic);
}
*/

void Worker::makeCrossField3d(const QImage &concavity) {
    if(this->crossfield3d != NULL) {
        delete this->crossfield3d;
    }
    this->crossfield3d = new CrossField3D(this->crossfield, concavity);
}

void Worker::rotateCrosses() {
    UnknownsIndexer index_harmonic = UnknownsIndexer(this->mask,2, this->crossfield);
    this->crossfield->rotateCrosses(this->pjumpfield, this->mask, &index_harmonic);
    this->crossfield->checkPositiveAngles(&index_harmonic);
}


// Places img on a white background as defined by mask.
QImage Worker::maskTheImage(QImage img) {

    QPainter p(&img);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.drawImage(0,0,this->maskImg);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    QImage white(img.width(), img.height(),QImage::Format_RGBA8888);
    white.fill(Qt::white);
    p.drawImage(0,0, white);
    p.end();
    return img;
}

QImage Worker::drawNormals() {
    rotateCrosses();
    makeCrossField3d(this->concavity);
    NormalField n(this->crossfield3d);
    return maskTheImage(n.toImage());
}

QImage Worker::drawShading() {
    rotateCrosses();
    makeCrossField3d(this->concavity);
    NormalField n(this->crossfield3d);
    return maskTheImage(n.toShadedImage());
}


QImage Worker::drawCrosses(int step) {
    // The simpler of the two crossfield visualizations in crossfieldgraphic.cpp
    Q_ASSERT(hasCrossField());

    int height = this->crossfield->height();
    int width = this->crossfield->width();

    QImage img(height, width, QImage::Format_RGBA8888);
    img.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&img);
    QPen pen(QColor(0,0,0), 3);
    painter.setPen(pen);

    for(int y=0; y<img.height(); y += step) { // Note: in my implementation, this is the crossfield's image size
        for(int x=0; x<img.width(); x += step) {
            if(true) { // Not masked
                if(!this->crossfield->isBoundaryPixel(x,y)) {
                    Vec2d v_0 = this->crossfield->getV0(x,y);
                    Vec2d v_1 = this->crossfield->getV1(x,y);
                    Vec2d v_2 = this->crossfield->getV2(x,y);
                    Vec2d v_3 = this->crossfield->getV3(x,y);

                    painter.drawLine(x,y, x+v_0(1)*(step/2), y+v_0(0)*(step/2));
                    painter.drawLine(x,y, x+v_1(1)*(step/2), y+v_1(0)*(step/2));
                    painter.drawLine(x,y, x+v_2(1)*(step/2), y+v_2(0)*(step/2));
                    painter.drawLine(x,y, x+v_3(1)*(step/2), y+v_3(0)*(step/2));
                }
            }
        }
    } // End for loops
    painter.end();

    return maskTheImage(img);
}

bool Worker::hasCrossField() const {
    return this->crossfield != NULL;
}

bool Worker::hasCrossField3d() const {
    return this->crossfield3d !=NULL;
}

