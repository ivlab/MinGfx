/** This small math library provides lightweight support for the
    graphics math needed inside MinVR. Some aspects (e.g., separate
    classes for Point3 and Vector3) are inspired the math libraries
    used in Brown Univ. computer graphics courses.  Also based on some
    routines introduced in the Hill & Kelley text used in UMN courses.
    Intended to be lightweight, for use inside MinVR only since
    application programmers will probably want to use the math package
    that is native to whatever graphics engine they are pairing with
    MinVR.
*/

#ifndef SRC_VECTOR3_H_
#define SRC_VECTOR3_H_

#include <iostream>

#include "point3.h"


namespace MinGfx {

    
/** @class Vector3
  * @brief 3D vector (magnitude and direction).
  */
class Vector3 {
public:

    /// Default constructor to create zero vector
    Vector3();

    /// Constructs a vector (x, y, z, 0)
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

    /// Returns the ith coordinate of the vector
    float operator[](const int i) const;

    /// Returns the ith coordinate of the vector
    float& operator[](const int i);  

    // --- Vector operations ---

    /// Returns "this dot v"
    float dot(const Vector3& v) const;

    /// Returns "this cross v"
    Vector3 cross(const Vector3& v) const;

    /// Returns the length of the vector
    float length() const;

    /// Normalizes the vector by making it unit length.  The result is returned
    /// AND 'this' vector is set equal to the result.
    Vector3 normalize();

    /// Returns a normalized (i.e., unit length) version of the vector without
    /// modifying the original 'this' vector.
    Vector3 unit() const;

    /// Returns a const pointer to the raw data array
    const float * value_ptr() const;
    
    /// Special vectors that are frequently needed
    static const Vector3& zero();
    static const Vector3& one();
    static const Vector3& unitX();
    static const Vector3& unitY();
    static const Vector3& unitZ();
    
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
