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

#ifndef SRC_MATRIX4_H_
#define SRC_MATRIX4_H_

#include <iostream>

#include "point3.h"
#include "vector3.h"


namespace MinGfx {


/** @class Matrix4
  * @brief A 4x4 transformation matrix
  */
class Matrix4 {
public: 

    /// Default constructor creates an identity matrix:
    Matrix4();

    /// Constructs a matrix given from an array of 16 floats in OpenGL matrix format
    /// (i.e., column major).
    Matrix4(const float* a);

    /// Copy constructor
    Matrix4(const Matrix4& m2);

    /// Destructor
    virtual ~Matrix4();

    /// Check for "equality", taking floating point imprecision into account
    bool operator==(const Matrix4& m2) const;

    /// Check for "inequality", taking floating point imprecision into account
    bool operator!=(const Matrix4& m2) const;

    /// Matrix assignment operator
    Matrix4& operator=(const Matrix4& m2);


    /// Returns a pointer to the raw data array used to store the matrix.  This
    /// is a 1D array of 16-elements stored in column-major order.
    const float * value_ptr() const;
    
    /// Accesses the ith element of the raw data array used to store the matrix.
    /// This is a 1D array of 16-elements stored in column-major order.
    float operator[](const int i) const;
    
    /// Accesses the ith element of the raw data array used to store the matrix.
    /// This is a 1D array of 16-elements stored in column-major order.
    float& operator[](const int i);
    
    /// Access an individual element of the array using the syntax:
    /// Matrix4 mat; float row1col2 = mat(1,2);
    float operator()(const int row, const int col) const;

    /// Access an individual element of the array using the syntax:
    /// Matrix4 mat; mat(1,2) = 1.0;
    float& operator()(const int row, const int col);
    
    
    /// Returns the c-th column of the matrix as a Vector type, e.g.,:
    /// Vector3 xAxis = mat.getColumnAsVector3(0);
    /// Vector3 yAxis = mat.getColumnAsVector3(1);
    /// Vector3 zAxis = mat.getColumnAsVector3(2);
    Vector3 getColumnAsVector3(int c) const;

    /// Returns the c-th column of the matrix as a Vector type, e.g.,:
    /// Point3 pos = mat.getColumnAsPoint3(3);
    Point3 getColumnAsPoint3(int c) const;


    // --- Static Constructors for Special Matrices ---
    
    /// Returns a matrix constructed from individual elements passed in row major
    /// order so that the matrix looks "correct" on the screen as you write this
    /// constructor on 4 lines of code as below.  Note the that internally the
    /// matrix constructed will be stored in a 16 element column major array to
    /// be consistent with OpenGL.
    static Matrix4 fromRowMajorElements(
        const float r1c1, const float r1c2, const float r1c3, const float r1c4,
        const float r2c1, const float r2c2, const float r2c3, const float r2c4,
        const float r3c1, const float r3c2, const float r3c3, const float r3c4,
        const float r4c1, const float r4c2, const float r4c3, const float r4c4
    );

    // --- Model Transformations ---
    
    /// Returns the scale matrix described by the vector
    static Matrix4 scale(const Vector3& v);

    /// Returns the translation matrix described by the vector
    static Matrix4 translation(const Vector3& v);

    /// Returns the rotation matrix about the x axis by the specified angle
    static Matrix4 rotationX(const float radians);

    /// Returns the rotation matrix about the y axis by the specified angle
    static Matrix4 rotationY(const float radians);

    /// Returns the rotation matrix about the z axis by the specified angle
    static Matrix4 rotationZ(const float radians);

    /// Returns the rotation matrix around the vector v placed at point p, rotate by angle a
    static Matrix4 rotation(const Point3& p, const Vector3& v, const float a);

    // --- View Matrices ---
    
    /// Returns a view matrix that centers the camera at the 'eye' position and
    /// orients it to look at the desired 'target' point with the top of the
    /// screen pointed as closely as possible in the direction of the 'up' vector.
    static Matrix4 lookAt(Point3 eye, Point3 target, Vector3 up);

    // --- Projection Matrices ---
    
    /// Returns a perspective projection matrix equivalent to the one gluPerspective
    /// creates.
    static Matrix4 perspective(float fovyInDegrees, float aspectRatio, float near, float far);
    
    /// Returns a projection matrix equivalent the one glFrustum creates
    static Matrix4 frustum(float left, float right, float bottom, float top, float near, float far);

    // --- Transpose, Inverse, and Other General Matrix Functions ---

    /// Returns an orthonormal version of the matrix, i.e., guarantees that the
    /// rotational component of the matrix is built from column vectors that are
    /// all unit vectors and orthogonal to each other.
    Matrix4 orthonormal() const;

    /// Returns the transpose of the matrix
    Matrix4 transpose() const;

    // Returns the determinant of the 3x3 matrix formed by excluding the specified row and column
    // from the 4x4 matrix.
    float subDeterminant(int excludeRow, int excludeCol) const;

    // Returns the cofactor matrix.
    Matrix4 cofactor() const;

    // Returns the determinant of the 4x4 matrix
    float determinant() const;

    // Returns the inverse of the 4x4 matrix if it is nonsingular.  If it is singular, then returns the
    // identity matrix. 
    Matrix4 inverse() const;

private:
    float m[16];
};



// ---------- Operator Overloads for Working with Points, Vectors, & Matrices ---------- 


// --- Matrix multiplication for Points, Vectors, & Matrices ---

/// Multiply matrix and scalar, returns the new matrix
Matrix4 operator*(const Matrix4& m, const float& s);

/// Multiply matrix and scalar, returns the new matrix
Matrix4 operator*(const float& s, const Matrix4& m);

/// Multiply matrix and point, returns the new point
Point3 operator*(const Matrix4& m, const Point3& p);

/// Multiply matrix and vector, returns the new vector
Vector3 operator*(const Matrix4& m, const Vector3& v);

/// Multiply two matrices, returns the result
Matrix4 operator*(const Matrix4& m1, const Matrix4& m2);


// --- Stream operators ---

std::ostream & operator<< ( std::ostream &os, const Matrix4 &m);
std::istream & operator>> ( std::istream &is, Matrix4 &m);

    
} // end namespace
 
#endif
