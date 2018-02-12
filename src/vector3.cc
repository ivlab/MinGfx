/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "vector3.h"

#include <math.h>

namespace mingfx {

static const Vector3 s_zerov3d = Vector3(0,0,0);
static const Vector3 s_onev3d = Vector3(1,1,1);
static const Vector3 s_unitxv3d = Vector3(1,0,0);
static const Vector3 s_unityv3d = Vector3(0,1,0);
static const Vector3 s_unitzv3d = Vector3(0,0,1);

const Vector3& Vector3::Zero() { return s_zerov3d; }
const Vector3& Vector3::One() { return s_onev3d; }
const Vector3& Vector3::UnitX() { return s_unitxv3d; }
const Vector3& Vector3::UnitY() { return s_unityv3d; }
const Vector3& Vector3::UnitZ() { return s_unitzv3d; }
    
    
Vector3::Vector3() {
    v[0] = 0.0;
    v[1] = 0.0;
    v[2] = 0.0;
}
  
Vector3::Vector3(float x, float y, float z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}
  
Vector3::Vector3(float *ptr) {
    v[0] = ptr[0];
    v[1] = ptr[1];
    v[2] = ptr[2];
}

Vector3::Vector3(const Vector3& other) {
    v[0] = other[0];
    v[1] = other[1];
    v[2] = other[2];
}
  
Vector3::~Vector3() {
}

bool Vector3::operator==(const Vector3& other) const {
  return (fabs(other[0] - v[0]) < MINGFX_MATH_EPSILON &&
          fabs(other[1] - v[1]) < MINGFX_MATH_EPSILON &&
          fabs(other[2] - v[2]) < MINGFX_MATH_EPSILON);
}
  
bool Vector3::operator!=(const Vector3& other) const {
    return (fabs(other[0] - v[0]) >= MINGFX_MATH_EPSILON &&
            fabs(other[1] - v[1]) >= MINGFX_MATH_EPSILON &&
            fabs(other[2] - v[2]) >= MINGFX_MATH_EPSILON);
}

Vector3& Vector3::operator=(const Vector3& other) {
    v[0] = other[0];
    v[1] = other[1];
    v[2] = other[2];
    return *this;
}
  
float Vector3::operator[](const int i) const {
    if ((i>=0) && (i<=2)) {
        return v[i];
    }
    else {
        // w component of a vector is 0 so return the constant 0.0
        return 0.0;
    }
}

float& Vector3::operator[](const int i) {
    return v[i];
}
  
float Vector3::Dot(const Vector3& other) const {
    return v[0]*other[0] + v[1]*other[1] + v[2]*other[2];
}

Vector3 Vector3::Cross(const Vector3& other) const {
    return Vector3(v[1] * other[2] - v[2] * other[1],
                   v[2] * other[0] - v[0] * other[2],
                   v[0] * other[1] - v[1] * other[0]);
}

float Vector3::Length() const {
    return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

void Vector3::Normalize() {
    // Hill & Kelley provide this:
    float sizeSq =  + v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    if (sizeSq < MINGFX_MATH_EPSILON) {
        return; // do nothing to zero vectors;
    } 
    float scaleFactor = (float)1.0/(float)sqrt(sizeSq);
    v[0] *= scaleFactor;
    v[1] *= scaleFactor;
    v[2] *= scaleFactor;
}


Vector3 Vector3::ToUnit() const {
    Vector3 v(*this);
    v.Normalize();
    return v;
}

const float * Vector3::value_ptr() const {
    return v;
}

    
    
Vector3 Vector3::Normalize(const Vector3 &v) {
    return v.ToUnit();
}

Vector3 Vector3::Cross(const Vector3 &v1, const Vector3 &v2) {
    return v1.Cross(v2);
}

float Vector3::Dot(const Vector3 &v1, const Vector3 &v2) {
    return v1.Dot(v2);
}
    
Vector3 Vector3::Lerp(const Vector3 &b, float alpha) const {
    float x = (1.0f-alpha)*(*this)[0] + alpha*b[0];
    float y = (1.0f-alpha)*(*this)[1] + alpha*b[1];
    float z = (1.0f-alpha)*(*this)[2] + alpha*b[2];
    return Vector3(x,y,z);
}

Vector3 Vector3::Lerp(const Vector3 &a, const Vector3 &b, float alpha) {
    float x = (1.0f-alpha)*a[0] + alpha*b[0];
    float y = (1.0f-alpha)*a[1] + alpha*b[1];
    float z = (1.0f-alpha)*a[2] + alpha*b[2];
    return Vector3(x,y,z);
}

    

Vector3 operator/(const Vector3& v, const float s) {
    const float invS = 1 / s;
    return Vector3(v[0]*invS, v[1]*invS, v[2]*invS);
}

Vector3 operator*(const float s, const Vector3& v) {
    return Vector3(v[0]*s, v[1]*s, v[2]*s);
}

Vector3 operator*(const Vector3& v, const float s) {
    return Vector3(v[0]*s, v[1]*s, v[2]*s);
}

Vector3 operator-(const Vector3& v) {
    return Vector3(-v[0], -v[1], -v[2]);
}

Point3 operator+(const Vector3& v, const Point3& p) {
    return Point3(p[0] + v[0], p[1] + v[1], p[2] + v[2]);
};

Point3 operator+(const Point3& p, const Vector3& v) {
    return Point3(p[0] + v[0], p[1] + v[1], p[2] + v[2]);
}

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
    return Vector3(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
}

Point3 operator-(const Point3& p, const Vector3& v) {
    return Point3(p[0] - v[0], p[1] - v[1], p[2] - v[2]);
}

Vector3 operator-(const Vector3& v1, const Vector3& v2) {
    return Vector3(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]);
}

Vector3 operator-(const Point3& p1, const Point3& p2) {
  return Vector3(p1[0] - p2[0], p1[1] - p2[1], p1[2] - p2[2]);
}


std::ostream & operator<< ( std::ostream &os, const Vector3 &v) {
  return os << "<" << v[0] << ", " << v[1] << ", " << v[2] << ">";
}

std::istream & operator>> ( std::istream &is, Vector3 &v) {
  // format:  <x, y, z>
  char dummy;
  return is >> dummy >> v[0] >> dummy >> v[1] >> dummy >> v[2] >> dummy;
}


} // end namespace
