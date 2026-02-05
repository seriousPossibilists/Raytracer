#ifndef RAY_H
#define RAY_H

#include <iostream>
#include <random>
#include "vec3.h"
#include "sphere.h"
#include "camera.h"

std::vector<Vec3> raytracer_data(width * height);
const int maxBounce = 5;
const int numRaysPerPixel = 150;
struct Ray {
    Vec3 dir;
    Vec3 origin;
};

hitInfo calc_ray_collision(Ray ray)
{
    double prevT = INFINITY; hitInfo prevInfo; prevInfo.didHit = false; prevInfo.hitObj.color= { 0.0, 0.0, 0.0 };
    for(size_t k = 0; k < spheres.size(); k++)
    {
        Sphere sphere = spheres[k];

        // (bx^2 + by^2)t^2 + 2(axbx + ayby)t + (ax^2+ay^2-r^2) = 0

        hitInfo hitInfo;
        hitInfo.hitObj = sphere;
        hitInfo.didHit = false;

        double a = dot(ray.dir, ray.dir);
        double b = 2.0 * dot(ray.origin - sphere.center, ray.dir);
        double c = dot(ray.origin - sphere.center, ray.origin - sphere.center) - (sphere.radius * sphere.radius);

        // Solving for the hit point 
        // Sphere normal is hitpoint - center

        double Δ = (b*b) - (4.0 * a * c);
        if (Δ < 0) 
            continue;

        double t = (-b - std::sqrt(Δ)) / (2.0*a);
        if(t <= 0 || t > prevT)
            continue;

        hitInfo.didHit = true;
        hitInfo.hitObj = sphere;
        hitInfo.hitPoint = ray.origin + (t * ray.dir);
        Vec3 normal = hitInfo.hitPoint - sphere.center;
        normal = normalize(normal);
        hitInfo.normalAtHitpoint = normal;
        prevT = t;
        prevInfo = hitInfo;
    }
    return prevInfo;
}

double random_normal_distri_val()
{
    static thread_local std::mt19937 gen(std::random_device{}());
    static thread_local std::normal_distribution<double> dist(0.0, 1.0);
    return dist(gen);
}

Vec3 random_hemisphere_reflection(Vec3 normal)
{
    double x = random_normal_distri_val();
    double y = random_normal_distri_val();
    double z = random_normal_distri_val();
    Vec3 dir = { x, y, z };
    if(dot(dir, normal) >= 0)
    {
        return normalize({ x, y, z });
    }
    else
    {
        return normalize({ -x, -y, -z });
    }
}

Vec3 trace(Ray ray)
{
    Vec3 incomingLight = {0.0, 0.0, 0.0};
    Vec3 rayColor = {1.0, 1.0, 1.0};
    for(size_t i = 0; i < maxBounce; i++)
    {
        hitInfo info = calc_ray_collision(ray);
        if(info.didHit)
        {
            ray.origin = info.hitPoint + (info.normalAtHitpoint * 0.0001);
            ray.dir = random_hemisphere_reflection(info.normalAtHitpoint);

            Vec3 emittedLight = info.hitObj.emissionColor * info.hitObj.emissionStrength;
            Vec3 contribution = emittedLight * rayColor;
            incomingLight = incomingLight + contribution;
            rayColor = rayColor * info.hitObj.color;
        }
        else
        {
            break;
        }
    }
    return incomingLight;
}

void process()
{
    size_t totalPixels = width * height;
    size_t pixelsProcessed = 0;
    for (size_t i = 0; i < width; i++)
    {
        for(size_t j = 0; j < height; j++)
        {
            Vec3 coord; 
            coord.x = static_cast<double>(i) / static_cast<double>(width); 
            coord.y = static_cast<double>(j) / static_cast<double>(height);
            Ray ray;
            ray.dir.x = (coord.x);
            ray.dir.y = (coord.y);
            ray.dir = 2.0 * ray.dir; ray.dir.x--; ray.dir.y--;
            ray.dir.x *= static_cast<double>(aspect);
            ray.dir.z = -1.0;
            ray.dir = normalize(ray.dir);

            ray.origin = cameraPosition;

            //hitInfo info = calc_ray_collision(ray);
            //raytracer_data[(width*j) + i] = info.hitObj.color;
            Vec3 totalLight;
            for(size_t h = 0; h < numRaysPerPixel; h++)
            {
                totalLight = totalLight + trace(ray);
            }
            raytracer_data[(width*j) + i] = totalLight * (1.0 / static_cast<double>(numRaysPerPixel));

            if(raytracer_data[(width*j) + i].x < 0) {raytracer_data[(width*j) + i].x = 0;}
            if(raytracer_data[(width*j) + i].y < 0) {raytracer_data[(width*j) + i].y = 0;}
            if(raytracer_data[(width*j) + i].z < 0) {raytracer_data[(width*j) + i].z = 0;}

            if(raytracer_data[(width*j) + i].x > 1.0) {raytracer_data[(width*j) + i].x = 1.0;}
            if(raytracer_data[(width*j) + i].y > 1.0) {raytracer_data[(width*j) + i].y = 1.0;}
            if(raytracer_data[(width*j) + i].z > 1.0) {raytracer_data[(width*j) + i].z = 1.0;}

            pixelsProcessed++;
            if (pixelsProcessed % 1000 == 0 || pixelsProcessed == totalPixels) // update every 1000 pixels
            {
                double progress = (100.0 * pixelsProcessed) / totalPixels;
                printf("\rRendering: %.2f%%", progress);
                fflush(stdout); // make sure it prints immediately
            }
        }
    }
}

#endif