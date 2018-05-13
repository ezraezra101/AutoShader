#include "crossfield3d.h"

CrossField3D::CrossField3D()
{
    Q_ASSERT(0=="Never call the default constructor - bad things may happen on destruction");
}


void cross2dTo3d(Vec2d v_0, Vec2d v_1, Vec3d *v_0_3, Vec3d *v_1_3)
{
    // TODO Oversimplified function.
    double dot = Vec2dOperations::dotProduct(v_0, v_1);
    double z = sqrt(abs(dot));
    *v_0_3 = Vec3dOperations::getNormalized(Vec3d(v_0(0), v_0(1), z));
    *v_1_3 = Vec3dOperations::getNormalized(Vec3d(v_0(0), v_0(1), dot <= 0 ? z : -z));
}

CrossField3D::CrossField3D(CrossField *cf) {
    this->w = cf->width();
    this->h = cf->height();

    this->v0_Field = new Vec3d[this->width() * this->height()];
    this->v1_Field = new Vec3d[this->width() * this->height()];
    for(int i=0; i < this->height(); i++) {
        for(int j=0; j < this->width(); j++) {
            cross2dTo3d(cf->getV0(i,j), cf->getV1(i,j),
                        &(this->v0_Field[index(j,i)]), &(this->v1_Field[index(j,i)]));
        }
    }
}

CrossField3D::~CrossField3D() {
    delete v0_Field;
    delete v1_Field;
}
