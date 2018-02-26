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

/** A quaternion to represent rotations in 3D space.  The main use of quaternions
 within the library is to support smooth interpolation between rotations, since
 this is not possible using Euler angles or rotation matrices.  The class includes
 a Slerp routine for spherical interpolation between rotations.  Example use:
 ~~~
 // find a rotation 1/2 way between r1 and r2, both originally expressed in Euler angles
 
 Vector3 euler1 = GfxMath::ToRadians(Vector3(0,0,60));
 Vector3 euler2 = GfxMath::ToRadians(Vector3(45,45,60));
 
 Quaternion q1 = Quaternion::FromEulerAnglesZYX(euler1);
 Quaternion q2 = Quaternion::FromEulerAnglesZYX(euler2);

 float alpha = 0.5;
 Quaternion q_half_way = q1.Slerp(q2, alpha);
 Vector3 new_euler_angles = GfxMath::ToDegrees(q_half_way.ToEulerAnglesZYX());
 ~~~
 */
class Quaternion {
public:
    /// Creates a quat with the identity rotation
    Quaternion();

    /// Creates a quat from the 4 parameters
    Quaternion(float qx, float qy, float qz, float qw);
    
    /// Creates a quate from a pointer to 4 floating point numbers in the order
    /// qx, qy, qz, qw.
    Quaternion(float *ptr);

    /// Copy constructor
    Quaternion(const Quaternion& other);
    
    virtual ~Quaternion();
    
    /// Check for "equality", taking floating point imprecision into account
    bool operator==(const Quaternion& q) const;
    
    /// Check for "inequality", taking floating point imprecision into account
    bool operator!=(const Quaternion& q) const;
    
    /// Assignment operator
    Quaternion& operator=(const Quaternion& q);
    
    /// Read only access to the ith coordinate of the quaternion (qx, qy, qz, qw).
    float operator[](const int i) const;
    
    /// Writable access the ith coordinate of the quaternion (qx, qy, qz, qw).
    float& operator[](const int i);
    
    /// Read only access to the x coordinate of the imaginary part of the quaternion.
    float x() const { return q[0]; }

    /// Read only access to the y coordinate of the imaginary part of the quaternion.
    float y() const { return q[1]; }
    
    /// Read only access to the z coordinate of the imaginary part of the quaternion.
    float z() const { return q[2]; }

    /// Read only access to the w, real part, of the quaternion.
    float w() const { return q[3]; }
    
    /// Returns a const pointer to the raw data array, stored in the order qx, qy, qz, qw.
    const float * value_ptr() const;

    /// Returns the dot product of this quaternion with another.
    float Dot(const Quaternion& q) const;

    /// Returns the length of the quaternion.
    float Length() const;
    
    /// Normalizes the quat by making it unit length.
    void Normalize();
    
    /// Returns a normalized (i.e., unit length) version of the quaternion without
    /// modifying the original.
    Quaternion ToUnit() const;
    
    /// Returns the conjugate of the quaternion.
    Quaternion Conjugate() const;
    
    /// Converts the rotation specified by the quaternion into Euler angles.
    Vector3 ToEulerAnglesZYX() const;

    /// Uses spherical interpolation to interpoloate between the rotation stored
    /// in this quaternion and the rotation stored in another.
    Quaternion Slerp(const Quaternion &other, float alpha) const;

    /// Creates a new quaternion that describes a rotation by angle radians about
    // the specified axis.
    static Quaternion FromAxisAngle(const Vector3 &axis, float angle);
    
    /// Creates a new quaternion from a rotation defined in Euler angles.
    static Quaternion FromEulerAnglesZYX(const Vector3 &angles);
    
    /// Uses spherical interpolation to interpoloate between the rotations
    /// specified by two quaternions.
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
