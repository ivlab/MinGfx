#include "gfxmath.h"

#include <algorithm>

namespace MinGfx {
    

float clamp(float x, float a, float b) {
    return std::min(std::max(x, a), b);
}
    
    
} // end namespace
