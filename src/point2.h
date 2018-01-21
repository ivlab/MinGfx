/**
 */

#ifndef SRC_POINT2_H_
#define SRC_POINT2_H_

#include <iostream>

namespace MinGfx {

// epsilon value used for == and != comparisons
#define MINGFX_MATH_EPSILON 1e-8


/** @class Point2
 * @brief 3D Point with floating point coordinates.
 */
class Point2 {
public:
    /// Default point at the origin
    Point2();
    
    /// Constructs a point given (x,y, 1)
    Point2(float x, float y);
    
    /// Constructs a point given a pointer to x,y,z data
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
    

    /// Special points that are frequently needed
    static const Point2& origin();
    static const Point2& zero();
    static const Point2& one();

private:
    float p[2];
};


std::ostream & operator<< ( std::ostream &os, const Point2 &p);
std::istream & operator>> ( std::istream &is, Point2 &p);

    
} // namespace

#endif
