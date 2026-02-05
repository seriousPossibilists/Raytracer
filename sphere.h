#ifndef SPHERE_H
#define SPHERE_H

#include <vector>
#include "vec3.h"

struct Sphere {
    double radius; 
    Vec3 color;
    Vec3 center;
    Vec3 emissionColor = {0.0, 0.0, 0.0};
    double emissionStrength = 0;
};

std::vector<Sphere> spheres;

struct hitInfo {
    bool didHit;
    Vec3 hitPoint = { 0.0, 0.0, 0.0 };
    Vec3 normalAtHitpoint;
    Sphere hitObj;
};

#endif