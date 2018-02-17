/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_QUATERNION_H_
#define SRC_QUATERNION_H_

#include <iostream>

#include "vector3.h"

namespace mingfx {

/**
 */
class Quaternion {
public:
    Quaternion();
    
    Quaternion(float qx, float qy, float qz, float qw);
    
    Quaternion(float *ptr);
    
    Quaternion(const Quaternion& other);
    
    virtual ~Quaternion();
    
    /// Check for "equality", taking floating point imprecision into account
    bool operator==(const Quaternion& q) const;
    
    /// Check for "inequality", taking floating point imprecision into account
    bool operator!=(const Quaternion& q) const;
    
    /// Assignment operator
    Quaternion& operator=(const Quaternion& q);
    
    /// Accesses the ith coordinate of the quaternion (qx, qy, qz, qw)
    float operator[](const int i) const;
    
    /// Accesses the ith coordinate of the quaternion (qx, qy, qz, qw)
    float& operator[](const int i);
    
    float x() const { return q[0]; }
    float y() const { return q[1]; }
    float z() const { return q[2]; }
    float w() const { return q[3]; }
    
    /// Returns a const pointer to the raw data array
    const float * value_ptr() const;
    
    float Dot(const Quaternion& q) const;
    
    float Length() const;
    
    /// Normalizes the quat by making it unit length.
    void Normalize();
    
    /// Returns a normalized (i.e., unit length) version of the quaternion without
    /// modifying the original.
    Quaternion ToUnit() const;
    
    /// Returns the conjugate of the quaternion.
    Quaternion Conjugate() const;
    
    Vector3 ToEulerAnglesZYX() const;
    
    Quaternion Slerp(const Quaternion &other, float alpha) const;
    
    static Quaternion FromAxisAngle(const Vector3 &axis, float angle);
    
    static Quaternion FromEulerAnglesZYX(const Vector3 &angles);
    
    static Quaternion Slerp(const Quaternion &a, const Quaternion &b, float alpha);
    
private:
    float q[4];
};


Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
Quaternion operator/(const Quaternion& q, const float s);
Quaternion operator*(const float s, const Quaternion& q);
Quaternion operator*(const Quaternion& q, const float s);
Quaternion operator-(const Quaternion& q);
Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
Quaternion operator-(const Quaternion& q1, const Quaternion& q2);

std::ostream & operator<< ( std::ostream &os, const Quaternion &q);
std::istream & operator>> ( std::istream &is, Quaternion &q);

    
} // end namespace


#endif
