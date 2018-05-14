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
            this->normals[this->index(j,i)] = NormalField::crossToNormal(v_0, v_1);
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
    n += Vec3d(1,1,1);
    n /= 2;
    return QColor((int)255*n(0), (int)255*n(1), (int)255*n(2));
}

QImage NormalField::toImage() {
    QImage img = QImage(this->height(), this->width(), QImage::Format_ARGB32);
    for(int y=0; y < this->height(); y++) {
        for(int x=0; x < this->width(); x++) {
            QColor color = this->getColor(x,y);

            img.setPixelColor(y,x,color); // Apparently somewhat slow...
        }
    }
    return img;
}

#define CLIP(x) ((x) < 0 ? 0 : ((x) > 1 ? 1 : (x)))
QColor shader(Vec3d n) {
    Vec3d l = Vec3dOperations::getNormalized(Vec3d(1,1,1));
    Vec3d diffuseColor = Vec3d(0.7,0.9, 0.9);

    Vec3d diffuseLight = Vec3dOperations::dotProduct(l, n) * diffuseColor;

    Vec3d light = diffuseLight;
    light(0) = CLIP(light(0));
    light(1) = CLIP(light(1));
    light(2) = CLIP(light(2));
    return QColor((int) (255*light(0)), (int) (255*light(1)), (int) (255*light(2)));
}

QImage NormalField::toShadedImage() {
    QImage img = QImage(this->height(), this->width(), QImage::Format_ARGB32); // Note: Performing transpose here!
    for(int y=0; y < this->height(); y++) {
        for(int x=0; x < this->width(); x++) {
            QColor color = shader(this->normals[this->index(x,y)]);

            img.setPixelColor(y,x,color); // Apparently somewhat slow...
        }
    }
    return img;
}
