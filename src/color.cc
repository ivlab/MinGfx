/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "color.h"

#include <math.h>

namespace mingfx {

Color::Color() {
    c[0] = 0.0;
    c[1] = 0.0;
    c[2] = 0.0;
    c[3] = 1.0;
}
  
Color::Color(float red, float green, float blue, float alpha) {
    c[0] = red;
    c[1] = green;
    c[2] = blue;
    c[3] = alpha;
}
  
Color::Color(float *ptr) {
    c[0] = ptr[0];
    c[1] = ptr[1];
    c[2] = ptr[2];
    c[3] = ptr[3];
}

Color::Color(const Color& other) {
    c[0] = other[0];
    c[1] = other[1];
    c[2] = other[2];
    c[3] = other[3];
}
  
Color::Color(const std::vector<float> &vals) {
    c[0] = vals[0];
    c[1] = vals[1];
    c[2] = vals[2];
    if (vals.size() > 3) {
        c[3] = vals[3];
    }
    else {
        c[3] = 1.0;
    }
}

    
Color::~Color() {
}
  
bool Color::operator==(const Color& other) const {
    return ((other[0] == c[0]) &&
            (other[1] == c[1]) &&
            (other[2] == c[2]) &&
            (other[3] == c[3]));
}
  
bool Color::operator!=(const Color& other) const {
    return ((other[0] != c[0]) ||
            (other[1] != c[1]) ||
            (other[2] != c[2]) ||
            (other[3] != c[3]));
}
  
Color& Color::operator=(const Color& other) {
    c[0] = other[0];
    c[1] = other[1];
    c[2] = other[2];
    c[3] = other[3];
    return *this;
}
  
float Color::operator[](const int i) const {
    return c[i];
}
  
float& Color::operator[](const int i) {
    return c[i];
}

    
const float * Color::value_ptr() const {
    return c;
}

std::vector<float> Color::ToVector() const {
    std::vector<float> v;
    v.push_back(c[0]);
    v.push_back(c[1]);
    v.push_back(c[2]);
    v.push_back(c[3]);
    return v;
}
    
    
Color Color::Lerp(const Color &b, float alpha) const {
    float red = (1.0f-alpha)*(*this)[0] + alpha*b[0];
    float grn = (1.0f-alpha)*(*this)[1] + alpha*b[1];
    float blu = (1.0f-alpha)*(*this)[2] + alpha*b[2];
    float alp = (1.0f-alpha)*(*this)[3] + alpha*b[3];
    return Color(red,grn,blu,alp);
}

Color Color::Lerp(const Color &a, const Color &b, float alpha) {
    float red = (1.0f-alpha)*a[0] + alpha*b[0];
    float grn = (1.0f-alpha)*a[1] + alpha*b[1];
    float blu = (1.0f-alpha)*a[2] + alpha*b[2];
    float alp = (1.0f-alpha)*a[3] + alpha*b[3];
    return Color(red,grn,blu,alp);
}

    
std::ostream & operator<< ( std::ostream &os, const Color &c) {
  return os << "(" << c[0] << ", " << c[1] << ", " << c[2] << ", " << c[3] << ")";
}

std::istream & operator>> ( std::istream &is, Color &c) {
  // format:  (r, g, b, a)
  char dummy;
  return is >> dummy >> c[0] >> dummy >> c[1] >> dummy >> c[2] >> dummy >> c[3] >> dummy;
}


} // end namespace
