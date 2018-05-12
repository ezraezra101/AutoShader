#ifndef NORMALFIELD_H
#define NORMALFIELD_H

#include "crossfield.h"
#include "vec3doperations.h"
#include "vec2doperations.h"

class NormalField
{
    Vec3d *normals;
    CrossField *crossfield;
public:
    NormalField();
    NormalField(CrossField *crossfield);
    ~NormalField();

    inline int height(){ return this->crossfield->height(); }
    inline int width(){ return this->crossfield->width(); }

    QColor getColor(int x,int y);

private:
    int index(int x, int y);

    // Converts a 2D cross (from a cross field) into a 3D one.
    static void cross2dTo3d(Vec2d v_0, Vec2d v_1, Vec3d *v_0_3, Vec3d *v_1_3);

    // Extract a normal from a 3d cross
    static Vec3d crossToNormal(Vec3d v_0, Vec3d v_1);
};

#endif // NORMALFIELD_H
