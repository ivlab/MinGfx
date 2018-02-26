/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "point3.h"
#include "vector3.h"

#include <math.h>

namespace mingfx {

static const Point3 s_zerop3d = Point3(0,0,0);
static const Point3 s_onep3d = Point3(1,1,1);

const Point3& Point3::Origin() { return s_zerop3d; }
const Point3& Point3::Zero() { return s_zerop3d; }
const Point3& Point3::One() { return s_onep3d; }
    
    
Point3::Point3() {
    p[0] = 0.0;
    p[1] = 0.0;
    p[2] = 0.0;
}
  
Point3::Point3(float x, float y, float z) {
    p[0] = x;
    p[1] = y;
    p[2] = z;
}
  
Point3::Point3(float *ptr) {
    p[0] = ptr[0];
    p[1] = ptr[1];
    p[2] = ptr[2];
}

Point3::Point3(const Point3& other) {
    p[0] = other[0];
    p[1] = other[1];
    p[2] = other[2];
}
  
Point3::~Point3() {
}
  
bool Point3::operator==(const Point3& other) const {
    return (fabs(other[0] - p[0]) < MINGFX_MATH_EPSILON &&
            fabs(other[1] - p[1]) < MINGFX_MATH_EPSILON &&
            fabs(other[2] - p[2]) < MINGFX_MATH_EPSILON);
}
  
bool Point3::operator!=(const Point3& other) const {
    return (fabs(other[0] - p[0]) >= MINGFX_MATH_EPSILON ||
            fabs(other[1] - p[1]) >= MINGFX_MATH_EPSILON ||
            fabs(other[2] - p[2]) >= MINGFX_MATH_EPSILON);
}
  
Point3& Point3::operator=(const Point3& other) {
    p[0] = other[0];
    p[1] = other[1];
    p[2] = other[2];
    return *this;
}
  
float Point3::operator[](const int i) const {
    if ((i>=0) && (i<=2)) {
        return p[i];
    }
    else {
        // w component of a point is 1 so return the constant 1.0
        return 1.0;
    }
}
  
float& Point3::operator[](const int i) {
    return p[i];
}

    
const float * Point3::value_ptr() const {
    return p;
}

Point3 Point3::Lerp(const Point3 &b, float alpha) const {
    float x = (1.0f-alpha)*(*this)[0] + alpha*b[0];
    float y = (1.0f-alpha)*(*this)[1] + alpha*b[1];
    float z = (1.0f-alpha)*(*this)[2] + alpha*b[2];
    return Point3(x,y,z);
}
    
Point3 Point3::Lerp(const Point3 &a, const Point3 &b, float alpha) {
    float x = (1.0f-alpha)*a[0] + alpha*b[0];
    float y = (1.0f-alpha)*a[1] + alpha*b[1];
    float z = (1.0f-alpha)*a[2] + alpha*b[2];
    return Point3(x,y,z);
}
    

float Point3::DistanceToPlane(const Point3 &plane_origin, const Vector3 &plane_normal) {
    return ((*this) - ClosestPointOnPlane(plane_origin, plane_normal)).Length();
}


Point3 Point3::ClosestPointOnPlane(const Point3 &plane_origin, const Vector3 &plane_normal) {
    Vector3 to_plane_origin = plane_origin - (*this);
    Vector3 inv_n = -plane_normal;
    if (to_plane_origin.Dot(inv_n) < 0.0) {
        inv_n = -inv_n;
    }
    
    Vector3 to_plane = inv_n * to_plane_origin.Dot(inv_n);
    return (*this) + to_plane;
}

Point3 Point3::ClosestPoint(const std::vector<Point3> &point_list) {
    int closest_id = 0;
    float closest_dist = (point_list[0] - *this).Length();
    for (int i=1; i<point_list.size(); i++) {
        float d = (point_list[i] - *this).Length();
        if (d < closest_dist) {
            closest_id = i;
            closest_dist = d;
        }
    }
    return point_list[closest_id];
}

    
    
std::ostream & operator<< ( std::ostream &os, const Point3 &p) {
  return os << "(" << p[0] << ", " << p[1] << ", " << p[2] << ")";
}

std::istream & operator>> ( std::istream &is, Point3 &p) {
  // format:  (x, y, z)
  char dummy;
  return is >> dummy >> p[0] >> dummy >> p[1] >> dummy >> p[2] >> dummy;
}


} // end namespace
