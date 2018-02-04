/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_POINT3_H_
#define SRC_POINT3_H_

#include <iostream>

namespace mingfx {

/// Epsilon value used for == and != comparisons within MinGfx
#define MINGFX_MATH_EPSILON 1e-8

    
/** A 3D Point with floating point coordinates, used for storing vertices and
 all sorts of other 3D graphics operations.  Point3s can be transformed by a
 Matrix4.  Example:
 ~~~
 Point3 a(0,0,1);
 std::cout << a << std::endl;
 Matrix4 M = Matrix4::Translation(Vector3(0,0,-1));
 Point3 b = M * a;
 std::cout << b << std::endl;
 ~~~
 */
class Point3 {
public:  
    /// Default point at the origin
    Point3();

    /// Constructs a point given (x,y,z,1), where the 1 comes from the use of
    /// homogeneous coordinates in computer graphics.
    Point3(float x, float y, float z);

    /// Constructs a point given a pointer to x,y,z data
    Point3(float *p);

    /// Copy constructor for point
    Point3(const Point3& p);

    /// Point destructor
    virtual ~Point3();

    /// Check for "equality", taking floating point imprecision into account
    bool operator==(const Point3& p) const;

    /// Check for "inequality", taking floating point imprecision into account
    bool operator!=(const Point3& p) const;

    /// Assignment operator
    Point3& operator=(const Point3& p);

    /// Accesses the ith coordinate of the point
    float operator[](const int i) const;

    /// Accesses the ith coordinate of the point
    float& operator[](const int i);

    /// Returns a const pointer to the raw data array
    const float * value_ptr() const;

    /// (0,0,0) - a shortcut for a special point that is frequently needed
    static const Point3& Origin();

    /// (0,0,0) - a shortcut for a special point that is frequently needed
    static const Point3& Zero();
    
    /// (1,1,1) - a shortcut for a special point that is frequently needed
    static const Point3& One();
    
private:
    float p[3];
};


std::ostream & operator<< ( std::ostream &os, const Point3 &p);
std::istream & operator>> ( std::istream &is, Point3 &p);


} // namespace
 
#endif
