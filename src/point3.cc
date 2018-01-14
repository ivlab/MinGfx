#include "point3.h"

#include <math.h>

namespace MinGfx {

Point3::Point3() {
    p[0] = 0.0;
    p[1] = 0.0;
    p[2] = 0.0;
}
  
Point3::Point3(float x, float y, float z) {
    p[0] = x;
    p[1] = y;
    p[2] = z;
}
  
Point3::Point3(float *ptr) {
    p[0] = ptr[0];
    p[1] = ptr[1];
    p[2] = ptr[2];
}

Point3::Point3(const Point3& other) {
    p[0] = other[0];
    p[1] = other[1];
    p[2] = other[2];
}
  
Point3::~Point3() {
}
  
bool Point3::operator==(const Point3& other) const {
    return (fabs(other[0] - p[0]) < MINGFX_MATH_EPSILON &&
            fabs(other[1] - p[1]) < MINGFX_MATH_EPSILON &&
            fabs(other[2] - p[2]) < MINGFX_MATH_EPSILON);
}
  
bool Point3::operator!=(const Point3& other) const {
    return (fabs(other[0] - p[0]) >= MINGFX_MATH_EPSILON ||
            fabs(other[1] - p[1]) >= MINGFX_MATH_EPSILON ||
            fabs(other[2] - p[2]) >= MINGFX_MATH_EPSILON);
}
  
Point3& Point3::operator=(const Point3& other) {
    p[0] = other[0];
    p[1] = other[1];
    p[2] = other[2];
    return *this;
}
  
float Point3::operator[](const int i) const {
    if ((i>=0) && (i<=2)) {
        return p[i];
    }
    else {
        // w component of a point is 1 so return the constant 1.0
        return 1.0;
    }
}
  
float& Point3::operator[](const int i) {
    return p[i];
}

    
const float * Point3::value_ptr() {
    return p;
}


std::ostream & operator<< ( std::ostream &os, const Point3 &p) {
  return os << "(" << p[0] << ", " << p[1] << ", " << p[2] << ")";
}

std::istream & operator>> ( std::istream &is, Point3 &p) {
  // format:  (x, y, z)
  char dummy;
  return is >> dummy >> p[0] >> dummy >> p[1] >> dummy >> p[2] >> dummy;
}


} // end namespace
