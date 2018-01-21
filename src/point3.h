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

#ifndef SRC_POINT3_H_
#define SRC_POINT3_H_

#include <iostream>

namespace MinGfx {

// epsilon value used for == and != comparisons
#define MINGFX_MATH_EPSILON 1e-8

    
/** @class Point3
  * @brief 3D Point with floating point coordinates.
  */
class Point3 {
public:  
    /// Default point at the origin
    Point3();

    /// Constructs a point given (x,y,z, 1)
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

    /// Special points that are frequently needed
    static const Point3& origin();
    static const Point3& zero();
    static const Point3& one();
    
private:
    float p[3];
};


std::ostream & operator<< ( std::ostream &os, const Point3 &p);
std::istream & operator>> ( std::istream &is, Point3 &p);


} // namespace
 
#endif
