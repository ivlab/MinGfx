/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_COLOR_H_
#define SRC_COLOR_H_

#include <iostream>
#include <vector>

namespace mingfx {

    
/** Represents a 4-component (R,G,B,A) color, stored internally in a float array
 to be compatable with OpenGL.  Example usage:
 ~~~
 Color black(0, 0, 0);
 Color white(1, 1, 1);
 Color gray(0.5, 0.5, 0.5);
 Color tranparent_red(1.0, 0.0, 0.0, 0.5);
 Color orange(1.0, 0.65, 0.0);
 
 float red_component_of_orange = orange[0];
 float green_component_of_orange = orange[1];
 float blue_component_of_orange = orange[2];
 float alpha_component_of_orange = orange[3];
 
 // Use value_ptr() to send the array of floats directly to OpenGL commands
 glColor4fv(orange.value_ptr());
 ~~~
 */
class Color {
public:  
    /// Defaults to black
    Color();

    /// Constructs a color.  Alpha defaults to 1.0 (completely opaque)
    Color(float red, float green, float blue, float alpha=1.0);

    /// Constructs a point given a pointer to float array
    Color(float *p);

    /// Constructs a point given a 3 or 4-element vector of floats
    Color(const std::vector<float> &vals);

    /// Copy constructor
    Color(const Color& p);

    /// Color destructor
    virtual ~Color();

    /// Check for equality
    bool operator==(const Color& p) const;

    /// Check for inequality
    bool operator!=(const Color& p) const;

    /// Assignment operator
    Color& operator=(const Color& p);

    /// Accesses the ith component of the color, stored in RGBA order.
    float operator[](const int i) const;

    /// Accesses the ith coordinate of the color, stored in RGBA order.
    float& operator[](const int i);

    /// Returns a const pointer to the raw data array
    const float * value_ptr() const;
    
    std::vector<float> ToVector() const;

    /// Linear interpolation between this color and another. Alpha=0.0 returns
    /// this color, and alpha=1.0 returns the other color, other values blend
    /// between the two via a linear interpolation on each color channel.
    Color Lerp(const Color &b, float alpha) const;
    
    /// Linear interpolation between two colors.  Alpha=0.0 returns 'a' and
    /// alpha=1.0 returns 'b', other values blend between the two via a linear
    /// interpolation on each color channel.
    static Color Lerp(const Color &a, const Color &b, float alpha);
    
 
private:
    float c[4];
};


std::ostream & operator<< ( std::ostream &os, const Color &c);
std::istream & operator>> ( std::istream &is, Color &c);


} // namespace
 
#endif
