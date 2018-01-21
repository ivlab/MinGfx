#ifndef SRC_PLATFORM_H_
#define SRC_PLATFORM_H_

#include <string>
#include <vector>

namespace MinGfx {
    
    
/** @class Platform
 *  @brief Filesystem access and other platform-specific routines
 */
class Platform {
public:
    
    /// True if filename is found and can be opened for reading on the system
    static bool fileExists(const std::string &filename);
    
    static std::string findFile(const std::string &basename, const std::vector<std::string> &searchpath);

    static std::string findFile(const std::string &basename, const std::string &searchpath);
    
    static std::string findMinGfxDataFile(const std::string &basename);
    
    static std::string findMinGfxShaderFile(const std::string &basename);
    
private:
};


} // end namespace

#endif
