/*
Copyright (c) 2017,2018 Regents of the University of Minnesota.
All Rights Reserved.
See corresponding header file for details.
*/

#define _USE_MATH_DEFINES
#include "quaternion.h"

#include "gfxmath.h"

namespace mingfx {


Quaternion::Quaternion() {
    q[0] = 0.0;
    q[1] = 0.0;
    q[2] = 0.0;
    q[3] = 1.0;
}

Quaternion::Quaternion(float qx, float qy, float qz, float qw) {
    q[0] = qx;
    q[1] = qy;
    q[2] = qz;
    q[3] = qw;
}

Quaternion::Quaternion(float *ptr) {
    q[0] = ptr[0];
    q[1] = ptr[1];
    q[2] = ptr[2];
    q[3] = ptr[3];
}

Quaternion::Quaternion(const Quaternion& other) {
    q[0] = other[0];
    q[1] = other[1];
    q[2] = other[2];
    q[3] = other[3];
}

Quaternion::~Quaternion() {
}

bool Quaternion::operator==(const Quaternion& other) const {
    return (fabs(other[0] - q[0]) < MINGFX_MATH_EPSILON &&
            fabs(other[1] - q[1]) < MINGFX_MATH_EPSILON &&
            fabs(other[2] - q[2]) < MINGFX_MATH_EPSILON &&
            fabs(other[3] - q[3]) < MINGFX_MATH_EPSILON);
}

bool Quaternion::operator!=(const Quaternion& other) const {
    return (fabs(other[0] - q[0]) >= MINGFX_MATH_EPSILON ||
            fabs(other[1] - q[1]) >= MINGFX_MATH_EPSILON ||
            fabs(other[2] - q[2]) >= MINGFX_MATH_EPSILON ||
            fabs(other[3] - q[3]) >= MINGFX_MATH_EPSILON);
}

Quaternion& Quaternion::operator=(const Quaternion& other) {
    q[0] = other[0];
    q[1] = other[1];
    q[2] = other[2];
    q[3] = other[3];
    return *this;
}

float Quaternion::operator[](const int i) const {
    if ((i>=0) && (i<=3)) {
        return q[i];
    }
    else {
        // this is an error!
        return 0.0;
    }
}

float& Quaternion::operator[](const int i) {
    return q[i];
}


const float * Quaternion::value_ptr() const {
    return q;
}

Quaternion Quaternion::Slerp(const Quaternion &other, float alpha) const {
    // https://en.wikipedia.org/wiki/Slerp
    
    Quaternion v0 = *this;
    Quaternion v1 = other;
    
    // Only unit quaternions are valid rotations.
    // Normalize to avoid undefined behavior.
    v0.Normalize();
    v1.Normalize();
    
    // Compute the cosine of the angle between the two vectors.
    float dot = v0.Dot(v1);
    
    // If the dot product is negative, the quaternions
    // have opposite handed-ness and slerp won't take
    // the shorter path. Fix by reversing one quaternion.
    if (dot < 0.0f) {
        v1 = -v1;
        dot = -dot;
    }
    
    const double DOT_THRESHOLD = 0.9995;
    if (dot > DOT_THRESHOLD) {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.
        
        Quaternion result = v0 + alpha*(v1 - v0);
        result.Normalize();
        return result;
    }
    
    GfxMath::Clamp(dot, -1, 1);        // Robustness: Stay within domain of acos()
    float theta_0 = acos(dot);        // theta_0 = angle between input vectors
    float theta = theta_0 * alpha;    // theta = angle between v0 and result
    
    float s0 = cos(theta) - dot * sin(theta) / sin(theta_0);  // == sin(theta_0 - theta) / sin(theta_0)
    float s1 = sin(theta) / sin(theta_0);
    
    return (s0 * v0) + (s1 * v1);
}

Quaternion Quaternion::Slerp(const Quaternion &a, const Quaternion &b, float alpha) {
    return a.Slerp(b, alpha);
}


std::ostream & operator<< ( std::ostream &os, const Quaternion &q) {
    return os << "<" << q[0] << ", " << q[1] << ", " << q[2] << ", " << q[3] << ")";
}

std::istream & operator>> ( std::istream &is, Quaternion &q) {
    // format:  <qx, qy, qz, qw>
    char dummy;
    return is >> dummy >> q[0] >> dummy >> q[1] >> dummy >> q[2] >> dummy >> q[3] >> dummy;
}


float Quaternion::Dot(const Quaternion& other) const {
    return q[0]*other[0] + q[1]*other[1] + q[2]*other[2] + q[3]*other[3];

}

float Quaternion::Length() const {
    return sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
}

void Quaternion::Normalize() {
    float sizeSq =  + q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
    if (sizeSq < MINGFX_MATH_EPSILON) {
        return; // do nothing to zero quats
    }
    float scaleFactor = (float)1.0/(float)sqrt(sizeSq);
    q[0] *= scaleFactor;
    q[1] *= scaleFactor;
    q[2] *= scaleFactor;
    q[3] *= scaleFactor;
}

Quaternion Quaternion::ToUnit() const {
    Quaternion qtmp(*this);
    qtmp.Normalize();
    return qtmp;
}

/// Returns the conjugate of the quaternion.
Quaternion Quaternion::Conjugate() const {
    return Quaternion(-q[0], -q[1], -q[2], q[3]);
}


Quaternion Quaternion::FromAxisAngle(const Vector3 &axis, float angle) {
    // [qx, qy, qz, qw] = [sin(a/2) * vx, sin(a/2)* vy, sin(a/2) * vz, cos(a/2)]
    float x = sin(angle/2.0) * axis[0];
    float y = sin(angle/2.0) * axis[1];
    float z = sin(angle/2.0) * axis[2];
    float w = cos(angle/2.0);
    return Quaternion(x,y,z,w);
}


Quaternion Quaternion::FromEulerAnglesZYX(const Vector3 &angles) {
    Quaternion rot_x = Quaternion::FromAxisAngle(Vector3::UnitX(), angles[0]);
    Quaternion rot_y = Quaternion::FromAxisAngle(Vector3::UnitY(), angles[1]);
    Quaternion rot_z = Quaternion::FromAxisAngle(Vector3::UnitZ(), angles[2]);
    return rot_z * rot_y * rot_x;
}

Vector3 Quaternion::ToEulerAnglesZYX() const {
    // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

    Vector3 angles;
    
    // roll (x-axis rotation)
    float sinr = +2.0 * (w() * x() + y() * z());
    float cosr = +1.0 - 2.0 * (x() * x() + y() * y());
    angles[0] = std::atan2(sinr, cosr);
    
    // pitch (y-axis rotation)
    float sinp = +2.0 * (w() * y() - z() * x());
    if (std::fabs(sinp) >= 1)
        angles[1] = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        angles[1] = std::asin(sinp);
    
    // yaw (z-axis rotation)
    float siny = +2.0 * (w() * z() + x() * y());
    float cosy = +1.0 - 2.0 * (y() * y() + z() * z());
    angles[2] = std::atan2(siny, cosy);
    
    return angles;
}


Quaternion operator*(const Quaternion& q1, const Quaternion& q2) {
    float real1 = q1[3];
    Vector3 imag1 = Vector3(q1[0], q1[1], q1[2]);
    
    float real2 = q2[3];
    Vector3 imag2 = Vector3(q2[0], q2[1], q2[2]);
    
    float real = real1*real2 - imag1.Dot(imag2);
    Vector3 imag = real1*imag2 + real2*imag1 + imag1.Cross(imag2);
    
    return Quaternion(imag[0], imag[1], imag[2], real);
}


Quaternion operator/(const Quaternion& q, const float s) {
    const float invS = 1.0 / s;
    return Quaternion(q[0]*invS, q[1]*invS, q[2]*invS, q[3]*invS);
}

Quaternion operator*(const float s, const Quaternion& q) {
    return Quaternion(q[0]*s, q[1]*s, q[2]*s, q[3]*s);
}

Quaternion operator*(const Quaternion& q, const float s) {
    return Quaternion(q[0]*s, q[1]*s, q[2]*s, q[3]*s);
}

Quaternion operator-(const Quaternion& q) {
    return Quaternion(-q[0], -q[1], -q[2], -q[3]);
}

Quaternion operator+(const Quaternion& q1, const Quaternion& q2) {
    return Quaternion(q1[0] + q2[0], q1[1] + q2[1], q1[2] + q2[2], q1[3] + q2[3]);
}

Quaternion operator-(const Quaternion& q1, const Quaternion& q2) {
    return Quaternion(q1[0] - q2[0], q1[1] - q2[1], q1[2] - q2[2], q1[3] - q2[3]);
}
    
} // end namespace
