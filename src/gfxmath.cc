#include "gfxmath.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <algorithm>

namespace MinGfx {
    

float clamp(float x, float a, float b) {
    return std::min(std::max(x, a), b);
}    

float to_radians(float degrees) {
    return degrees * M_PI / 180.0;
}

float to_degrees(float radians) {
    return radians * 180.0 / M_PI;
}
    
} // end namespace
