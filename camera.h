#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"

const int width = 1500;
const float aspect = 16.f / 9.f;
const int height = static_cast<int>(width * (1/aspect));
Vec3 cameraPosition = { 0.0, 0.0, 7.0 };

#endif