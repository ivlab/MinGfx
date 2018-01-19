#ifndef SRC_PLATFORM_H_
#define SRC_PLATFORM_H_

#include <string>

namespace MinGfx {
    
    
/** @class Platform
 *  @brief Filesystem access and other platform-specific routines
 */
class Platform {
public:
    
    /// True if filename is found and can be opened for reading on the system
    static bool fileExists(std::string filename);
    
private:
};


} // end namespace

#endif
