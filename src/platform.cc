
#include "platform.h"

#ifdef WIN32
    #include <windows.h>
#else
    #include <sys/stat.h>
#endif


namespace MinGfx {
    
bool Platform::fileExists(std::string filename) {
#ifdef WIN32
    DWORD attr = ::GetFileAttributesW(filename.c_str());
    return (attr != 0xFFFFFFFF);
#else
    struct stat buf;
    return (stat(filename.c_str(), &buf) == 0);
#endif
}
   

} // end namespace
