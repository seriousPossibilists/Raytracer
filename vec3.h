#ifndef VEC3_H
#define VEC3_H

#include <cmath>

struct Vec3 {
    double x {};
    double y {};
    double z {};

    Vec3 operator*(double t) const { 
        return { x * t, y * t, z * t }; 
    }
};

inline Vec3 operator+(const Vec3& u, const Vec3& v) {
    return { u.x + v.x, u.y + v.y, u.z + v.z };
}

inline Vec3 operator-(const Vec3& u, const Vec3& v) {
    return { u.x - v.x, u.y - v.y, u.z - v.z };
}

inline Vec3 operator*(const Vec3 &u, const Vec3& v) {
    return { v.x * u.x, v.y * u.y, v.z * u.z };
}

inline Vec3 operator*(double t, const Vec3& v) {
    return { v.x * t, v.y * t, v.z * t };
}

inline double dot(const Vec3& u, const Vec3& v) {
    return (u.x * v.x) + (u.y * v.y) + (u.z * v.z);
}

inline Vec3 normalize(const Vec3& v) {
    double length = std::sqrt(dot(v, v));
    return { v.x / length, v.y / length, v.z / length };
}

inline Vec3 cross(const Vec3 &u, const Vec3 &v) {
        return Vec3{
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    };
}
#endif 
