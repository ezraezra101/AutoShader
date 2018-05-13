#include "normalfield.h"

NormalField::NormalField()
{
    Q_ASSERT(0=="Never call the default constructor - bad things may happen on destruction");
}

NormalField::NormalField(CrossField3D *crossfield)
{
    Q_ASSERT(crossfield != NULL);
    this->w = crossfield->width();
    this->h = crossfield->height();

    this->normals = new Vec3d[this->width() * this->height()];
    for(int i=0; i < this->height(); i++) {
        for(int j=0; j < this->width(); j++) {
            Vec3d v_0 = crossfield->getV0(i,j);
            Vec3d v_1 = crossfield->getV1(i,j);
            this->normals[this->index(i,j)] = NormalField::crossToNormal(v_0, v_1);
        }
    }
}

NormalField::~NormalField() {
    delete normals;
}

int NormalField::index(int x, int y) {
    return this->width() * y + x;
}

Vec3d NormalField::crossToNormal(Vec3d v_0, Vec3d v_1)
{
    Vec3d normal = Vec3dOperations::crossProduct(v_0, v_1);
    if(normal(3) < 0) { // Backfacing normal
        normal = -normal;
    }
    return Vec3dOperations::getNormalized(normal); // Probably not necessary (assuming this function has been given unit length perpendicular vectors)
}

QColor NormalField::getColor(int x, int y) {
    Vec3d n = this->normals[this->index(x,y)];
    return QColor((int)255*(n(0)+1)/2, (int)255*(n(1)+1)/2, (int)255*(n(2)+1)/2, 255);
}

QImage NormalField::toImage() {
    QImage img = QImage(this->height(), this->width(), QImage::Format_ARGB32);
    for(int i=0; i < img.height(); i++) {
        for(int j=0; j < img.width(); j++) {
            QColor color = this->getColor(i,j);

            img.setPixelColor(j,i,color); // Apparently somewhat slow...
        }
    }
    return img;
}
