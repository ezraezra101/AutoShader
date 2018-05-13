#ifndef NORMALFIELD_H
#define NORMALFIELD_H

#include "crossfield3d.h"
#include "vec3doperations.h"

class NormalField
{
    Vec3d *normals;
    int w, h;
public:
    NormalField();
    NormalField(CrossField3D *crossfield);
    ~NormalField();

    inline int height(){ return this->h; }
    inline int width(){ return this->w; }

    QColor getColor(int x,int y);
    QImage toImage();

private:
    int index(int x, int y);

    // Extract a normal from a 3d cross
    static Vec3d crossToNormal(Vec3d v_0, Vec3d v_1);
};

#endif // NORMALFIELD_H
