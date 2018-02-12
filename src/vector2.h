/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_VECTOR2_H_
#define SRC_VECTOR2_H_

#include <iostream>

#include "point2.h"


namespace mingfx {


/** A 2D Vector with floating point coordinates, used for storing 2D translations,
 mouse movements, and screen-space vectors.
 */
class Vector2 {
public:
    
    /// Default constructor to create zero vector
    Vector2();
    
    /// Constructs a vector (x,y,0), where the 0 comes from the use of
    /// homogeneous coordinates in computer graphics.
    Vector2(float x, float y);
    
    /// Constructs a vector given a pointer to x,y,z data
    Vector2(float *v);
    
    /// Copy constructor for vector
    Vector2(const Vector2& v);
    
    /// Vector destructor
    virtual ~Vector2();
    
    /// Check for "equality", taking floating point imprecision into account
    bool operator==(const Vector2& v) const;
    
    /// Check for "inequality", taking floating point imprecision into account
    bool operator!=(const Vector2& v) const;
    
    /// Vector assignment operator
    Vector2& operator=(const Vector2& v);
    
    /// Returns the ith coordinate of the vector
    float operator[](const int i) const;
    
    /// Returns the ith coordinate of the vector
    float& operator[](const int i);
    
    // --- Vector operations ---
    
    /// Returns "this dot v"
    float Dot(const Vector2& v) const;
    
    /// Returns the length of the vector
    float Length() const;
    
    /// Normalizes the vector by making it unit length.
    void Normalize();
    
    /// Returns a normalized (i.e., unit length) version of the vector without
    /// modifying the original ('this') vector.
    Vector2 ToUnit() const;
    
    /// Linear interpolation between this vector and another. Alpha=0.0 returns
    /// this vector, and alpha=1.0 returns the other vector, other values blend
    /// between the two.
    Vector2 Lerp(const Vector2 &b, float alpha) const;
    
    /// Returns a const pointer to the raw data array
    const float * value_ptr() const;

    
    
    /// Returns a new vector that is the unit version of v.
    static Vector2 Normalize(const Vector2 &v);
    
    /// Returns v1 dot v2
    static float Dot(const Vector2 &v1, const Vector2 &v2);
    
    /// (0,0) - a shortcut for a special vector that is frequently needed
    static const Vector2& Zero();

    /// (1,1) - a shortcut for a special vector that is frequently needed
    static const Vector2& One();

    /// (1,0) - a shortcut for a special vector that is frequently needed
    static const Vector2& UnitX();

    /// (0,1) - a shortcut for a special vector that is frequently needed
    static const Vector2& UnitY();
    
    /// Linear interpolation between two vectors.  Alpha=0.0 returns 'a' and
    /// alpha=1.0 returns 'b', other values blend between the two.
    static Vector2 Lerp(const Vector2 &a, const Vector2 &b, float alpha);

private:
    float v[2];
};


// ---------- Operator Overloads for Working with Vectors ----------


// --- Scalers ---

/// Divide the vector by the scalar s
Vector2 operator/(const Vector2& v, const float s);

/// Multiply the vector by the scalar s
Vector2 operator*(const float s, const Vector2& v);

/// Multiply the vector by the scalar s
Vector2 operator*(const Vector2& v, const float s);

/// Negate the vector
Vector2 operator-(const Vector2& v);

// Note: no -(point) operator, that's an undefined operation


// --- Point and Vector Arithmetic ---

/// Adds a vector and a point, returns a point
Point2 operator+(const Vector2& v, const Point2& p);

/// Adds a point and a vector, returns a point
Point2 operator+(const Point2& p, const Vector2& v);

/// Adds a vector and a vector, returns a vector
Vector2 operator+(const Vector2& v1, const Vector2& v2);

// Note: no (point + point) operator, that's an undefined operation

/// Subtracts a vector from a point, returns a point
Point2 operator-(const Point2& p, const Vector2& v);

/// Subtracts v2 from v1, returns a vector
Vector2 operator-(const Vector2& v1, const Vector2& v2);

/// Returns the vector spanning p1 and p2
Vector2 operator-(const Point2& p1, const Point2& p2);

// Note: no (vector - point) operator, that's an undefined operation


// --- Stream operators ---

// Vector2
std::ostream & operator<< ( std::ostream &os, const Vector2 &v);
std::istream & operator>> ( std::istream &is, Vector2 &v);

    
} // end namespace

#endif
