
#ifndef SRC_COLOR_H_
#define SRC_COLOR_H_

#include <iostream>

namespace MinGfx {

    
/** @class Color
  * @brief Represents a 4-component (R,G,B,A) color, stored internally in a
     float array to be compatable with OpenGL.
  */
class Color {
public:  
    /// Defaults to black
    Color();

    /// Constructs a color.  Alpha defaults to 1.0 (completely opaque)
    Color(float red, float green, float blue, float alpha=1.0);

    /// Constructs a point given a pointer to float array
    Color(float *p);

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

private:
    float c[4];
};


std::ostream & operator<< ( std::ostream &os, const Color &c);
std::istream & operator>> ( std::istream &is, Color &c);


} // namespace
 
#endif
