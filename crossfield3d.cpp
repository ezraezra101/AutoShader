#include "crossfield3d.h"

CrossField3D::CrossField3D()
{
    Q_ASSERT(0=="Never call the default constructor - bad things may happen on destruction");
}


void cross2dTo3d(Vec2d v_0, Vec2d v_1, bool flip, Vec3d *v_0_3, Vec3d *v_1_3)
{
    // TODO Oversimplified function.
    double dot = Vec2dOperations::dotProduct(v_0, v_1);
    double z = sqrt(abs(dot));
    *v_0_3 = Vec3dOperations::getNormalized(Vec3d(v_0(0), v_0(1), z));
    *v_1_3 = Vec3dOperations::getNormalized(Vec3d(v_1(0), v_1(1), dot <= 0 ? z : -z));

    if(flip) {
        (*v_0_3)(2) = -(*v_0_3)(2);
    }

}

CrossField3D::CrossField3D(CrossField *cf, const QImage &concavityGuidance) {
    this->w = cf->width();
    this->h = cf->height();

    this->v0_Field = new Vec3d[this->width() * this->height()];
    this->v1_Field = new Vec3d[this->width() * this->height()];

    for(int y=0; y < this->height(); y++) {
        for(int x=0; x < this->width(); x++) {
            cross2dTo3d(
                        cf->getV0(y,x), cf->getV1(y,x),

                        QColor(concavityGuidance.pixel(y,x)) != Qt::transparent,
                        &(this->v0_Field[index(x,y)]), &(this->v1_Field[index(x,y)]));
        }
    }
}

CrossField3D::~CrossField3D() {
    delete v0_Field;
    delete v1_Field;
}
