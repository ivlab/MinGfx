/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2017, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_MATRIX4_H_
#define SRC_MATRIX4_H_

#include <iostream>

#include "point3.h"
#include "vector3.h"
#include "ray.h"


namespace mingfx {


/** A 4x4 transformation matrix stored internally as an array of floats in
 column-major order so as to be compatible with OpenGL.  Examples:
 ~~~
 // constructing various matrices:
 Matrix4 T = Matrix4::Translation(Vector3(1,0,0));
 Matrix4 S = Matrix4::Scale(Vector3(2,2,2));
 Matrix4 R = Matrix4::RotateX(GfxMath::toRadians(45.0));

 // compose matrices together by multiplication
 Matrix4 M = T * R * S;
 Matrix4 Minv = M.Inverse();
 
 // transforming points, vectors, etc.
 Point3 p1(1,1,1);
 Point3 p2 = M * p1;
 
 Vector3 v1(1,1,1);
 Vector3 v2 = M * v1;

 Ray r1(p1, v1);
 Ray r2 = M * r1;
 ~~~
 */
class Matrix4 {
public: 

    /// The default constructor creates an identity matrix:
    Matrix4();

    /// Constructs a matrix given from an array of 16 floats in OpenGL matrix format
    /// (i.e., column major).
    Matrix4(const float* a);
    
    /// Constructs a matrix given from a vector of 16 floats in OpenGL matrix format
    /// (i.e., column major).
    Matrix4(const std::vector<float> &a);

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
    Vector3 ColumnToVector3(int c) const;

    /// Returns the c-th column of the matrix as a Vector type, e.g.,:
    /// Point3 pos = mat.getColumnAsPoint3(3);
    Point3 ColumnToPoint3(int c) const;
    
    std::vector<float> ToVector() const;


    // --- Static Constructors for Special Matrices ---
    
    /** Returns a matrix constructed from individual elements passed in row major
     order so that the matrix looks "correct" on the screen as you write this
     constructor on 4 lines of code as below.  Note the that internally the
     matrix constructed will be stored in a 16 element column major array to
     be consistent with OpenGL.
     */
    static Matrix4 FromRowMajorElements(
        const float r1c1, const float r1c2, const float r1c3, const float r1c4,
        const float r2c1, const float r2c2, const float r2c3, const float r2c4,
        const float r3c1, const float r3c2, const float r3c3, const float r3c4,
        const float r4c1, const float r4c2, const float r4c3, const float r4c4
    );

    // --- Model Transformations ---
    
    /// Returns the scale matrix described by the vector
    static Matrix4 Scale(const Vector3 &v);

    /// Returns the translation matrix described by the vector
    static Matrix4 Translation(const Vector3 &v);

    /// Returns the rotation matrix about the x axis by the specified angle
    static Matrix4 RotationX(const float radians);

    /// Returns the rotation matrix about the y axis by the specified angle
    static Matrix4 RotationY(const float radians);

    /// Returns the rotation matrix about the z axis by the specified angle
    static Matrix4 RotationZ(const float radians);

    /// Returns the rotation matrix around the vector v placed at point p, rotate by angle a
    static Matrix4 Rotation(const Point3 &p, const Vector3 &v, const float a);

    /// Creates a transformation matrix that maps a coordinate space, *a*, defined
    /// one point, *a_p*, and two vectors, *a_v1* and *a_v2*, to a new coordinate
    /// space, *b*, also defined by one point, *b_p*, and two vectors, *b_v1* and
    /// *b_v2*.  The transformation will thus include both some rotation and some
    /// translation.  Pseudocode example of aligning a billboard defined in the
    /// XY plane with a normal in the +Z direction and that rotates around the Y
    /// axis with a camera:
    /// ~~~
    /// // define a coordiante space for a canonical billboard geometry defined
    /// // right at the origin.
    /// Point3 a_p = Point3(0,0,0);     // billboard's initial base position
    /// Vector3 a_v1 = Vector3(0,1,0);  // billboard's initial up direction
    /// Vector3 a_v2 = Vector3(0,0,1);  // billboard's initial normal direction
    ///
    /// // define a coordinate space for where we want this billboard to go and
    /// // the direction it should be facing
    /// Point3 b_p = desired_base_pos;  // new position for the billboard
    /// Vector3 b_v1 = Vector3(0,1,0);  // +Y is still up, doesn't change
    /// Vector3 b_v2 = (camera.eye() - desired_base_pos); // the normal should point toward the camera
    /// b_v2[1] = 0.0;    // with 0 change in Y so the billboard does not tilt
    /// b_v2.Normalize(); // convert to a unit vector
    ///
    /// Matrix4 billboard_model_matrix = Matrix4::Align(a_p, a_v1, a_v2,   b_p, b_v1, b_v2);
    /// ~~~
    static Matrix4 Align(const Point3 &a_p, const Vector3 &a_v1, const Vector3 &a_v2,
                         const Point3 &b_p, const Vector3 &b_v1, const Vector3 &b_v2);

    
    // --- View Matrices ---
    
    /** Returns a view matrix that centers the camera at the 'eye' position and
     orients it to look at the desired 'target' point with the top of the
     screen pointed as closely as possible in the direction of the 'up' vector.
     */
    static Matrix4 LookAt(Point3 eye, Point3 target, Vector3 up);

    // --- Projection Matrices ---
    
    /// Returns a perspective projection matrix equivalent to the one gluPerspective
    /// creates.
    static Matrix4 Perspective(float fovyInDegrees, float aspectRatio, float near, float far);
    
    /// Returns a projection matrix equivalent the one glFrustum creates
    static Matrix4 Frustum(float left, float right, float bottom, float top, float near, float far);

    // --- Inverse, Transposeand Other General Matrix Functions ---

    /// Returns the inverse of the 4x4 matrix if it is nonsingular.  If it is
    /// singular, then returns the identity matrix.
    Matrix4 Inverse() const;
    
    /** Returns an orthonormal version of the matrix, i.e., guarantees that the
     rotational component of the matrix is built from column vectors that are
     all unit vectors and orthogonal to each other.
     */
    Matrix4 Orthonormal() const;

    /// Returns the transpose of the matrix
    Matrix4 Transpose() const;

    /// Returns the determinant of the 3x3 matrix formed by excluding the specified
    /// row and column from the 4x4 matrix.
    float SubDeterminant(int excludeRow, int excludeCol) const;

    /// Returns the cofactor matrix.
    Matrix4 Cofactor() const;

    /// Returns the determinant of the 4x4 matrix
    float Determinant() const;



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


    
/// Multiply matrix and the point and vector portions of the ray, returns the new ray
Ray operator*(const Matrix4& m, const Ray& r);
    
// --- Stream operators ---

std::ostream & operator<< ( std::ostream &os, const Matrix4 &m);
std::istream & operator>> ( std::istream &is, Matrix4 &m);

    
} // end namespace
 
#endif
