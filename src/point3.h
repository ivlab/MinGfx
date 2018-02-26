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
#include <vector>

namespace mingfx {

/// Epsilon value used for == and != comparisons within MinGfx
#define MINGFX_MATH_EPSILON 1e-8

// forward declaration
class Vector3;
    
    
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

    /// Read only access to the ith coordinate of the point.
    float operator[](const int i) const;
    
    /// Returns a reference to the ith coordinate of the point.  Use this
    /// accessor if you wish to set the coordinate rather than just request
    /// its value.  Example:
    /// ~~~
    /// Point3 a;
    /// a[0] = 5.0; // set the x-coordinate of the point
    /// ~~~
    float& operator[](const int i);
    
    /// Read only access to the x coordinate.  Can also use my_point[0].  Use
    /// the my_point[0] = 1.0; form if you need to set the value.
    float x() const { return p[0]; }
    
    /// Read only access to the y coordinate.  Can also use my_point[1].  Use
    /// the my_point[1] = 1.0; form if you need to set the value.
    float y() const { return p[1]; }
    
    /// Read only access to the z coordinate.  Can also use my_point[2].  Use
    /// the my_point[2] = 1.0; form if you need to set the value.
    float z() const { return p[2]; }
    
    /// In homogeneous coordinates, the w coordinate for all points is 1.0.
    float w() const { return 1.0; }
    
    /// Returns a const pointer to the raw data array
    const float * value_ptr() const;

    
    /// Linear interpolation between this point and another. Alpha=0.0 returns
    /// this point, and alpha=1.0 returns the other point, other values blend
    /// between the two.
    Point3 Lerp(const Point3 &b, float alpha) const;

    /// Returns the shortest (i.e., perpendicular) distance from this point to
    /// a plane defined by a point and a normal.
    float DistanceToPlane(const Point3 &plane_origin, const Vector3 &plane_normal);

    /// Returns the perpendicular projection of this point onto a plane defined
    /// by a point and a normal.
    Point3 ClosestPointOnPlane(const Point3 &plane_origin, const Vector3 &plane_normal);

    /// Given a list of points, returns the closest in the last to the current point.
    Point3 ClosestPoint(const std::vector<Point3> &point_list);
    
    
    
    /// (0,0,0) - a shortcut for a special point that is frequently needed
    static const Point3& Origin();

    /// (0,0,0) - a shortcut for a special point that is frequently needed
    static const Point3& Zero();
    
    /// (1,1,1) - a shortcut for a special point that is frequently needed
    static const Point3& One();
    
    /// Linear interpolation between two points.  Alpha=0.0 returns 'a' and
    /// alpha=1.0 returns 'b', other values blend between the two.
    static Point3 Lerp(const Point3 &a, const Point3 &b, float alpha);
    
    
    
private:
    float p[3];
};


std::ostream & operator<< ( std::ostream &os, const Point3 &p);
std::istream & operator>> ( std::istream &is, Point3 &p);


} // namespace
 
#endif
