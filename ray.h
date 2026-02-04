#ifndef RAY_H
#define RAY_H

#include "vec3.h"
#include "sphere.h"

struct Ray {
    Vec3 dir;
    Vec3 origin;
};

Vec3 ray_trace(Vec3 coord, float aspect, int width, int height, Sphere sphere, double *prevZ)
{
    // (bx^2 + by^2)t^2 + 2(axbx + ayby)t + (ax^2+ay^2-r^2) = 0
    Ray ray;
    Vec3 lightDirection; lightDirection.x = 1.0; lightDirection.y = 1.0; lightDirection.z = 1.0;
    lightDirection = normalize(lightDirection);

    ray.dir.x = (coord.x);
    ray.dir.y = (coord.y);
    ray.dir.z = -1.0;
    ray.dir = 2.0 * ray.dir; ray.dir.x--; ray.dir.y--;
    ray.dir.x *= aspect;
    ray.dir = normalize(ray.dir);

    ray.origin.x = 0.0; ray.origin.y = 0.0; ray.origin.z = 2.0;

    double a = dot(ray.dir, ray.dir);
    double b = 2.0 * dot(ray.origin - sphere.center, ray.dir);
    double c = dot(ray.origin - sphere.center, ray.origin - sphere.center) - (sphere.radius * sphere.radius);
    Vec3 result; result.x = 0; result.y = 0; result.z = 0;

    // Solving for the hit point (assuming closest hitpoint as valid)
    // Sphere normal is hitpoint - center
    double Δ = (b*b) - (4.0*a*c);
    if (Δ < 0) {return result;}
    double t = (-b - std::sqrt(Δ)) / (2.0*a);
    if (t > 0)
    {
        Vec3 normal = (ray.origin + (t*ray.dir)) - sphere.center;
        double hitPointZ = (normal + sphere.center).z;
        if(hitPointZ > *prevZ) { *prevZ = hitPointZ; }
        normal = normalize(normal);
        if (dot(lightDirection, normal) > 0)
        {
            result.x = sphere.color.x * dot(lightDirection, normal);
            result.y = sphere.color.y * dot(lightDirection, normal);
            result.z = sphere.color.z * dot(lightDirection, normal);
            return result;
        }
    }
    return result;
};

#endif