#ifndef CROSSFIELD3D_H
#define CROSSFIELD3D_H

#include "crossfield.h"
#include "vec2doperations.h"
#include "vec3doperations.h"

class CrossField3D
{
    Vec3d *v0_Field;
    Vec3d *v1_Field;
    int w, h;

    inline int index(int x, int y) { return this->width() * y + x; }

public:
    CrossField3D();
    CrossField3D(CrossField *, const QImage &convexityGuidance);
    ~CrossField3D();

    inline int height(){ return this->h; }
    inline int width(){ return this->w; }

    inline Vec3d getV0(int i, int j) { return this->v0_Field[i*this->width()+j]; }
    inline Vec3d getV1(int i, int j) { return this->v1_Field[i*this->width()+j]; }
    inline Vec3d getV2(int i, int j) { return -(this->v0_Field[i*this->width()+j]); }
    inline Vec3d getV3(int i, int j) { return -(this->v1_Field[i*this->width()+j]); }
};

#endif // CROSSFIELD3D_H
