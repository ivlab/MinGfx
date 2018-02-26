/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_POINT2_H_
#define SRC_POINT2_H_

#include <iostream>

namespace mingfx {

/// Epsilon value used for == and != comparisons within MinGfx
#define MINGFX_MATH_EPSILON 1e-8


/** A 2D Point with floating point coordinates, used for storing 2D texture
 coordinates, screen-space graphics, and mouse input.
 */
class Point2 {
public:
    /// Default point at the origin
    Point2();
    
    /// Constructs a point given (x,y,1), where the 1 comes from the use of
    /// homogeneous coordinates in computer graphics.
    Point2(float x, float y);
    
    /// Constructs a point given a pointer to x,y data
    Point2(float *p);
    
    /// Copy constructor for point
    Point2(const Point2& p);
    
    /// Point destructor
    virtual ~Point2();
    
    /// Check for "equality", taking floating point imprecision into account
    bool operator==(const Point2& p) const;
    
    /// Check for "inequality", taking floating point imprecision into account
    bool operator!=(const Point2& p) const;
    
    /// Assignment operator
    Point2& operator=(const Point2& p);
    
    /// Read only access to the ith coordinate of the point.
    float operator[](const int i) const;
    
    /// Returns a reference to the ith coordinate of the point.  Use this
    /// accessor if you wish to set the coordinate rather than just request
    /// its value.  Example:
    /// ~~~
    /// Point2 a;
    /// a[0] = 5.0; // set the x-coordinate of the point
    /// ~~~
    float& operator[](const int i);
    
    /// Read only access to the x coordinate.  Can also use my_point[0].  Use
    /// the my_point[0] = 1.0; form if you need to set the value.
    float x() const { return p[0]; }
    
    /// Read only access to the y coordinate.  Can also use my_point[1].  Use
    /// the my_point[1] = 1.0; form if you need to set the value.
    float y() const { return p[1]; }
    
    /// In homogeneous coordinates, the w coordinate for all points is 1.0.
    float w() const { return 1.0; }
    
    
    /// Returns a const pointer to the raw data array
    const float * value_ptr() const;
    
    /// Linear interpolation between this point and another. Alpha=0.0 returns
    /// this point, and alpha=1.0 returns the other point, other values blend
    /// between the two.
    Point2 Lerp(const Point2 &b, float alpha) const;


    /// (0,0) - a shortcut for a special point that is frequently needed
    static const Point2& Origin();

    /// (0,0) - a shortcut for a special point that is frequently needed
    static const Point2& Zero();
    
    /// (1,1) - a shortcut for a special point that is frequently needed
    static const Point2& One();

    /// Linear interpolation between two points.  Alpha=0.0 returns 'a' and
    /// alpha=1.0 returns 'b', other values blend between the two.
    static Point2 Lerp(const Point2 &a, const Point2 &b, float alpha);
    
private:
    float p[2];
};


std::ostream & operator<< ( std::ostream &os, const Point2 &p);
std::istream & operator>> ( std::istream &is, Point2 &p);

    
} // namespace

#endif
