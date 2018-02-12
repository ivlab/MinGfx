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
    
    /// Accesses the ith coordinate of the point
    float operator[](const int i) const;
    
    /// Accesses the ith coordinate of the point
    float& operator[](const int i);
    
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
