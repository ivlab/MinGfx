/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "vector2.h"

#include <math.h>

namespace mingfx {
    
    
static const Vector2 s_zerov2d = Vector2(0,0);
static const Vector2 s_onev2d = Vector2(1,1);
static const Vector2 s_unitxv2d = Vector2(1,0);
static const Vector2 s_unityv2d = Vector2(0,1);

const Vector2& Vector2::Zero() { return s_zerov2d; }
const Vector2& Vector2::One() { return s_onev2d; }
const Vector2& Vector2::UnitX() { return s_unitxv2d; }
const Vector2& Vector2::UnitY() { return s_unityv2d; }

Vector2::Vector2() {
    v[0] = 0.0;
    v[1] = 0.0;
}

Vector2::Vector2(float x, float y) {
    v[0] = x;
    v[1] = y;
}

Vector2::Vector2(float *ptr) {
    v[0] = ptr[0];
    v[1] = ptr[1];
}

Vector2::Vector2(const Vector2& other) {
    v[0] = other[0];
    v[1] = other[1];
}

Vector2::~Vector2() {
}

bool Vector2::operator==(const Vector2& other) const {
    return (fabs(other[0] - v[0]) < MINGFX_MATH_EPSILON &&
            fabs(other[1] - v[1]) < MINGFX_MATH_EPSILON);
}

bool Vector2::operator!=(const Vector2& other) const {
    return (fabs(other[0] - v[0]) >= MINGFX_MATH_EPSILON &&
            fabs(other[1] - v[1]) >= MINGFX_MATH_EPSILON);
}

Vector2& Vector2::operator=(const Vector2& other) {
    v[0] = other[0];
    v[1] = other[1];
    return *this;
}

float Vector2::operator[](const int i) const {
    if ((i>=0) && (i<=1)) {
        return v[i];
    }
    else {
        // w component of a vector is 0 so return the constant 0.0
        return 0.0;
    }
}

float& Vector2::operator[](const int i) {
    return v[i];
}

float Vector2::Dot(const Vector2& other) const {
    return v[0]*other[0] + v[1]*other[1];
}

float Vector2::Length() const {
    return sqrt(v[0]*v[0] + v[1]*v[1]);
}

void Vector2::Normalize() {
    // Hill & Kelley provide this:
    float sizeSq =  + v[0]*v[0] + v[1]*v[1];
    if (sizeSq < MINGFX_MATH_EPSILON) {
        return; // do nothing to zero vectors;
    }
    float scaleFactor = (float)1.0/(float)sqrt(sizeSq);
    v[0] *= scaleFactor;
    v[1] *= scaleFactor;
}

Vector2 Vector2::ToUnit() const {
    Vector2 v(*this);
    v.Normalize();
    return v;
}


Vector2 Vector2::Lerp(const Vector2 &b, float alpha) const {
    float x = (1.0f-alpha)*(*this)[0] + alpha*b[0];
    float y = (1.0f-alpha)*(*this)[1] + alpha*b[1];
    return Vector2(x,y);
}

Vector2 Vector2::Lerp(const Vector2 &a, const Vector2 &b, float alpha) {
    float x = (1.0f-alpha)*a[0] + alpha*b[0];
    float y = (1.0f-alpha)*a[1] + alpha*b[1];
    return Vector2(x,y);
}

    
const float * Vector2::value_ptr() const {
    return v;
}

    
Vector2 Vector2::Normalize(const Vector2 &v) {
    return v.ToUnit();
}


float Vector2::Dot(const Vector2 &v1, const Vector2 &v2) {
    return v1.Dot(v2);
}
    


Vector2 operator/(const Vector2& v, const float s) {
    const float invS = 1 / s;
    return Vector2(v[0]*invS, v[1]*invS);
}

Vector2 operator*(const float s, const Vector2& v) {
    return Vector2(v[0]*s, v[1]*s);
}

Vector2 operator*(const Vector2& v, const float s) {
    return Vector2(v[0]*s, v[1]*s);
}

Vector2 operator-(const Vector2& v) {
    return Vector2(-v[0], -v[1]);
}

Point2 operator+(const Vector2& v, const Point2& p) {
    return Point2(p[0] + v[0], p[1] + v[1]);
};

Point2 operator+(const Point2& p, const Vector2& v) {
    return Point2(p[0] + v[0], p[1] + v[1]);
}

Vector2 operator+(const Vector2& v1, const Vector2& v2) {
    return Vector2(v1[0] + v2[0], v1[1] + v2[1]);
}

Point2 operator-(const Point2& p, const Vector2& v) {
    return Point2(p[0] - v[0], p[1] - v[1]);
}

Vector2 operator-(const Vector2& v1, const Vector2& v2) {
    return Vector2(v1[0] - v2[0], v1[1] - v2[1]);
}

Vector2 operator-(const Point2& p1, const Point2& p2) {
    return Vector2(p1[0] - p2[0], p1[1] - p2[1]);
}


std::ostream & operator<< ( std::ostream &os, const Vector2 &v) {
    return os << "<" << v[0] << ", " << v[1] << ">";
}

std::istream & operator>> ( std::istream &is, Vector2 &v) {
    // format:  <x, y, z>
    char dummy;
    return is >> dummy >> v[0] >> dummy >> v[1] >> dummy;
}

    
} // end namespace
