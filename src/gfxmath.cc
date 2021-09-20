/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "gfxmath.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

#include "ray.h"


namespace mingfx {
    
const float GfxMath::PI = 3.14159265359f;
const float GfxMath::TWO_PI = 6.28318530718f;
const float GfxMath::HALF_PI = 1.57079632679f;

float GfxMath::sin(float a) {
#ifdef WIN32
    return std::sinf(a);
#else
    return std::sin(a);
#endif
}

float GfxMath::cos(float a) {
#ifdef WIN32
    return std::cosf(a);
#else
    return std::cos(a);
#endif
}

float GfxMath::tan(float a) {
#ifdef WIN32
    return std::tanf(a);
#else
    return std::tan(a);
#endif
}

float GfxMath::asin(float a) {
#ifdef WIN32
    return std::asinf(a);
#else
    return std::asin(a);
#endif
}

float GfxMath::acos(float a) {
#ifdef WIN32
    return std::acosf(a);
#else
    return std::acos(a);
#endif
}

float GfxMath::atan(float a) {
#ifdef WIN32
    return std::atanf(a);
#else
    return std::atan(a);
#endif
}

float GfxMath::atan2(float a, float b) {
#ifdef WIN32
    return std::atan2f(a, b);
#else
    return std::atan2(a, b);
#endif
}

float GfxMath::Clamp(float x, float a, float b) {
    return std::min(std::max(x, a), b);
}    

float GfxMath::ToRadians(float degrees) {
    return degrees * GfxMath::PI / 180.0f;
}

float GfxMath::ToDegrees(float radians) {
    return radians * 180.0f / GfxMath::PI;
}

Vector3 GfxMath::ToRadians(Vector3 degrees) {
    return Vector3(ToRadians(degrees[0]), ToRadians(degrees[1]), ToRadians(degrees[2]));
}

Vector3 GfxMath::ToDegrees(Vector3 radians) {
    return Vector3(ToDegrees(radians[0]), ToDegrees(radians[1]), ToDegrees(radians[2]));
}
    
float GfxMath::Lerp(float a, float b, float alpha) {
    return (1.0f-alpha)*a + alpha*b;
}
    
int GfxMath::iLerp(int a, int b, float alpha) {
    return (int)std::round((1.0f-alpha)*(float)a + alpha*(float)b);
}
    
Point3 GfxMath::ScreenToNearPlane(const Matrix4 &V, const Matrix4 &P, const Point2 &ndcPoint) {
    Matrix4 filmPtToWorld = (P*V).Inverse();
    return filmPtToWorld * Point3(ndcPoint[0], ndcPoint[1], -1.0);
}
    
    
Point3 GfxMath::ScreenToWorld(const Matrix4 &V, const Matrix4 &P, const Point2 &ndcPoint, float zValue) {
    Matrix4 filmPtToWorld = (P*V).Inverse();
    float zneg1topos1 = zValue*2.0f - 1.0f;
    return filmPtToWorld * Point3(ndcPoint[0], ndcPoint[1], zneg1topos1);
}

    
Point3 GfxMath::ScreenToDepthPlane(const Matrix4 &V, const Matrix4 &P, const Point2 &ndcPoint, float planeDepth) {
    Point3 pNear = ScreenToNearPlane(V, P, ndcPoint);
    
    Matrix4 camMat = V.Inverse();
    Point3 eye = camMat.ColumnToPoint3(3);
    Vector3 look = -camMat.ColumnToVector3(2);
    
    Ray r(eye, pNear - eye);
    
    Point3 p3D;
    float t;
    if (!r.IntersectPlane(eye + planeDepth*look, -look, &t, &p3D)) {
        std::cerr << "filmplane2D_to_plane3D() error -- no intersection found!" << std::endl;
    }
    return p3D;
}
    
    
} // end namespace
