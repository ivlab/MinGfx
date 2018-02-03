#include "gfxmath.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

#include "ray.h"


namespace MinGfx {
    

float clamp(float x, float a, float b) {
    return std::min(std::max(x, a), b);
}    

float to_radians(float degrees) {
    return degrees * M_PI / 180.0;
}

float to_degrees(float radians) {
    return radians * 180.0 / M_PI;
}
    
    
Point3 filmplane2D_to_nearplane3D(const Matrix4 &V, const Matrix4 &P, const Point2 &ndcPoint) {
    Matrix4 filmPtToWorld = (P*V).inverse();
    return filmPtToWorld * Point3(ndcPoint[0], ndcPoint[1], -1.0);
}
    
    
Point3 filmplane2D_to_world3D(const Matrix4 &V, const Matrix4 &P, const Point2 &ndcPoint, float zValue) {
    Matrix4 filmPtToWorld = (P*V).inverse();
    float zneg1topos1 = zValue*2.0 - 1.0;
    return filmPtToWorld * Point3(ndcPoint[0], ndcPoint[1], zneg1topos1);
}

    
Point3 filmplane2D_to_plane3D(const Matrix4 &V, const Matrix4 &P, const Point2 &ndcPoint, float planeDepth) {
    Point3 pNear = filmplane2D_to_nearplane3D(V, P, ndcPoint);
    
    Matrix4 camMat = V.inverse();
    Point3 eye = camMat.getColumnAsPoint3(3);
    Vector3 look = -camMat.getColumnAsVector3(2);
    
    Ray r(eye, pNear - eye);
    
    Point3 p3D;
    float t;
    if (!r.IntersectPlane(eye + planeDepth*look, -look, &t, &p3D)) {
        std::cerr << "filmplane2D_to_plane3D() error -- no intersection found!" << std::endl;
    }
    return p3D;
}
    
    
} // end namespace
