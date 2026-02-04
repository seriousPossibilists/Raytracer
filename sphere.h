#ifndef SPHERE_H
#define SPHERE_H

#include "vec3.h"

struct Sphere {
    double radius; 
    Vec3 color;
    Vec3 center;
    bool isLuminous;
};

#endif