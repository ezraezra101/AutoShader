#include "normalfield.h"

NormalField::NormalField()
{
    this->crossfield = NULL;
}

NormalField::NormalField(CrossField *crossfield)
{
    this->crossfield = crossfield;
    Q_ASSERT(crossfield != NULL);
    this->normals = new Vec3d[this->width() * this->height()];
    for(int i=0; i < this->height(); i++) {
        for(int j=0; j < this->width(); j++) {
            Vec2d v_0 = crossfield->getV0(i,j);
            Vec2d v_1 = crossfield->getV0(i,j);

            Vec3d v3_0, v3_1;
            NormalField::cross2dTo3d(v_0, v_1, &v3_0, &v3_1);
            this->normals[this->index(i,j)] = NormalField::crossToNormal(v3_0, v3_1);
        }
    }
}

NormalField::~NormalField() {
    delete normals;
}

int NormalField::index(int x, int y) {
    Q_ASSERT(crossfield != NULL);
    return this->crossfield->width() * y + x;
}

void NormalField::cross2dTo3d(Vec2d v_0, Vec2d v_1, Vec3d *v_0_3, Vec3d *v_1_3)
{

    // TODO This should be represented by its own (complicated) class that performs optimization as described in the paper.
    double dot = Vec2dOperations::dotProduct(v_0, v_1);
    double z = sqrt(abs(dot));
    *v_0_3 = Vec3dOperations::getNormalized(Vec3d(v_0(0), v_0(1), z));
    *v_1_3 = Vec3dOperations::getNormalized(Vec3d(v_0(0), v_0(1), dot <= 0 ? z : -z));


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
    return QColor((int)255*(n(0)+1)/2, (int)255*(n(0)+1)/2, (int)255*(n(0)+1)/2);
}
