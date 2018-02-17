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
    

float GfxMath::Clamp(float x, float a, float b) {
    return std::min(std::max(x, a), b);
}    

float GfxMath::ToRadians(float degrees) {
    return degrees * M_PI / 180.0;
}

float GfxMath::ToDegrees(float radians) {
    return radians * 180.0 / M_PI;
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
    return std::round((1.0f-alpha)*(float)a + alpha*(float)b);
}
    
Point3 GfxMath::ScreenToNearPlane(const Matrix4 &V, const Matrix4 &P, const Point2 &ndcPoint) {
    Matrix4 filmPtToWorld = (P*V).Inverse();
    return filmPtToWorld * Point3(ndcPoint[0], ndcPoint[1], -1.0);
}
    
    
Point3 GfxMath::ScreenToWorld(const Matrix4 &V, const Matrix4 &P, const Point2 &ndcPoint, float zValue) {
    Matrix4 filmPtToWorld = (P*V).Inverse();
    float zneg1topos1 = zValue*2.0 - 1.0;
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
