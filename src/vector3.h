/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_VECTOR3_H_
#define SRC_VECTOR3_H_

#include <iostream>

#include "point3.h"


namespace mingfx {

    
/** A 3D Vector with floating point coordinates, used for storing normals and
 all sorts of other 3D graphics operations.  Vector3s can be transformed by a
 Matrix4, and a Vector3 can be created by subtracting two Point3s.  Example:
 ~~~
 // subtracting two points creates a vector
 Point3 a(0,0,0);
 Point3 b(2,0,0);
 Vector3 c = b - a;

 // vectors can be transformed by Matrix4s
 Vector3 dir = c.ToUnit();
 Matrix4 M = Matrix4::RotateX(GfxMath::ToDegrees(30.0));
 Vector3 dir_transformed = M * dir;

 // vectors can be added and subtracted
 Vector3 d(1,0,0);
 Vector3 e = c + d;
 
 // and we can do the usual dot products and cross products too
 Vector3 f = d.Dot(e);
 Vector3 g = b.Cross(d);
 ~~~
 */
class Vector3 {
public:

    /// Default constructor to create zero vector
    Vector3();

    /// Constructs a vector (x,y,z,0), where the 0 comes from the use of
    /// homogeneous coordinates in computer graphics
    Vector3(float x, float y, float z);

    /// Constructs a vector given a pointer to x,y,z data
    Vector3(float *v);

    /// Copy constructor for vector
    Vector3(const Vector3& v);

    /// Vector destructor
    virtual ~Vector3();

    /// Check for "equality", taking floating point imprecision into account
    bool operator==(const Vector3& v) const;

    /// Check for "inequality", taking floating point imprecision into account
    bool operator!=(const Vector3& v) const;

    /// Vector assignment operator
    Vector3& operator=(const Vector3& v);

    /// Read only access to the ith coordinate of the vector.
    float operator[](const int i) const;

    /// Returns a reference to the ith coordinate of the vector.  Use this
    /// accessor if you wish to set the coordinate rather than just request
    /// its value.  Example:
    /// ~~~
    /// Vector3 a;
    /// a[0] = 5.0; // set the x-coordinate of the vector
    /// ~~~
    float& operator[](const int i);  

    /// Read only access to the x coordinate.  Can also use my_vector[0].  Use
    /// the my_vector[0] = 1.0; form if you need to set the value.
    float x() const { return v[0]; }
    
    /// Read only access to the y coordinate.  Can also use my_vector[1].  Use
    /// the my_vector[1] = 1.0; form if you need to set the value.
    float y() const { return v[1]; }

    /// Read only access to the z coordinate.  Can also use my_vector[2].  Use
    /// the my_vector[2] = 1.0; form if you need to set the value.
    float z() const { return v[2]; }

    /// In homogeneous coordinates, the w coordinate for all vectors is 0.0.
    float w() const { return 0.0; }
    
    
    // --- Vector operations ---

    /** Returns "this dot v", for example:
     ~~~
     Vector3 a(1,0,0);
     Vector3 b(0.5,0,0);
     Vector3 c = a.Dot(b);
     ~~~
     */
    float Dot(const Vector3& v) const;

    /** Returns "this cross v", for example:
    ~~~
    Vector3 x(1,0,0);
    Vector3 y(0,1,0);
    Vector3 z = x.Cross(y);
    ~~~
    */
    Vector3 Cross(const Vector3& v) const;

    /// Returns the length of the vector
    float Length() const;

    /// Normalizes the vector by making it unit length.
    void Normalize();

    /// Returns a normalized (i.e., unit length) version of the vector without
    /// modifying the original 'this' vector.
    Vector3 ToUnit() const;

    /// Returns a const pointer to the raw data array
    const float * value_ptr() const;

    /// Linear interpolation between this vector and another. Alpha=0.0 returns
    /// this vector, and alpha=1.0 returns the other vector, other values blend
    /// between the two.
    Vector3 Lerp(const Vector3 &b, float alpha) const;
    
    
    /// (0,0,0) - a shortcut for a special vector that is frequently needed
    static const Vector3& Zero();

    /// (1,1,1) - a shortcut for a special vector that is frequently needed
    static const Vector3& One();

    /// (1,0,0) - a shortcut for a special vector that is frequently needed
    static const Vector3& UnitX();

    /// (0,1,0) - a shortcut for a special vector that is frequently needed
    static const Vector3& UnitY();

    /// (0,0,1) - a shortcut for a special vector that is frequently needed
    static const Vector3& UnitZ();
    

    /** Returns a new vector that is the unit version of v.  This is just an
     alternative syntax for ToUnit().  Example:
     ~~~
     Vector3 a(100,150,80);
     Vector3 b = Vector3::Normalize(a);
     Vector3 c = a.ToUnit();
     // b and c are the same.
     ~~~
     */
    static Vector3 Normalize(const Vector3 &v);
    
    /** Returns v1 cross v2.  This is just an alternative syntax for Cross().
     Example:
     ~~~
     Vector3 x(1,0,0);
     Vector3 y(0,1,0);
     Vector3 z1 = Vector3::Cross(x,y);
     Vector3 z2 = x.Cross(y);
     // z1 and z2 are the same.
     ~~~
     */
    static Vector3 Cross(const Vector3 &v1, const Vector3 &v2);
    
    /** Returns v1 dot v2.  This is just an alternative syntax for Dot().
     Example:
     ~~~
     Vector3 a(1,0,0);
     Vector3 b(0.5,0,0);
     Vector3 c1 = a.Dot(b);
     Vector3 c2 = Vector3::Dot(a,b);
     // c1 and c2 are the same.
     ~~~
     */
    static float Dot(const Vector3 &v1, const Vector3 &v2);
    
    /// Linear interpolation between two vectors.  Alpha=0.0 returns 'a' and
    /// alpha=1.0 returns 'b', other values blend between the two.
    static Vector3 Lerp(const Vector3 &a, const Vector3 &b, float alpha);
    
private:
    float v[3];
};


// ---------- Operator Overloads for Working with Vectors ----------


// --- Scalers ---

/// Divide the vector by the scalar s
Vector3 operator/(const Vector3& v, const float s);

/// Multiply the vector by the scalar s
Vector3 operator*(const float s, const Vector3& v);

/// Multiply the vector by the scalar s
Vector3 operator*(const Vector3& v, const float s);

/// Negate the vector
Vector3 operator-(const Vector3& v);

// Note: no -(point) operator, that's an undefined operation


// --- Point and Vector Arithmetic ---

/// Adds a vector and a point, returns a point
Point3 operator+(const Vector3& v, const Point3& p);

/// Adds a point and a vector, returns a point
Point3 operator+(const Point3& p, const Vector3& v);

/// Adds a vector and a vector, returns a vector
Vector3 operator+(const Vector3& v1, const Vector3& v2);

// Note: no (point + point) operator, that's an undefined operation

/// Subtracts a vector from a point, returns a point
Point3 operator-(const Point3& p, const Vector3& v);

/// Subtracts v2 from v1, returns a vector
Vector3 operator-(const Vector3& v1, const Vector3& v2);

/// Returns the vector spanning p1 and p2
Vector3 operator-(const Point3& p1, const Point3& p2);

// Note: no (vector - point) operator, that's an undefined operation

    
    
    
// --- Stream operators ---

// Vector3
std::ostream & operator<< ( std::ostream &os, const Vector3 &v);
std::istream & operator>> ( std::istream &is, Vector3 &v);

    
} // end namespace
 
#endif
