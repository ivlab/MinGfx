/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#ifndef SRC_PLATFORM_H_
#define SRC_PLATFORM_H_

#include <string>
#include <vector>

namespace mingfx {
    
    
/** Provides access to the underlying file system and other platform-specific 
 routines.
 */
class Platform {
public:
    
    /// True if filename is found and can be opened for reading on the system
    static bool FileExists(const std::string &filename);
 
    /* Looks for a file named basename in each of the paths specified.  If found,
     the full path to the file is returned.  If not found, then basename is returned.
     Example:
     ~~~
     std::vector<std::string> search_path;
     search_path.push_back(".");
     search_path.push_back("./data");
     search_path.push_back("./shaders");
     search_path.push_back("/usr/local/share/blah/blah/data");
     
     std::string file = Platform::findFile("mydata.csv", search_path);
     ~~~
     */
    static std::string FindFile(const std::string &basename, const std::vector<std::string> &searchpath);

    /* Looks for a file named basename in each of the paths specified in a semi-colon
     separated list.  If found, the full path to the file is returned.  If not found, 
     then basename is returned. Example:
     ~~~
     std::string search_path = ".;./data;./shaders;/usr/local/share/blah/blah/data";
     std::string file = Platform::findFile("mydata.csv", search_path);
     ~~~
     */
    static std::string FindFile(const std::string &basename, const std::string &searchpath);

    /** Searches for a data file that ships with MinGfx.  This will look in the
     following locations in order:
     1. the current working directory.
     2. a subdirectory called data within the current working directory.
     3. the installed data directory INSTALL_PREFIX/share/MinGfx-1.0/data.
     4. the data directory in the MinGfx build tree.
     If the file is found, the full path is returned, else basename is returned.
     */
    static std::string FindMinGfxDataFile(const std::string &basename);

    /** Searches for a shader file that ships with MinGfx.  This will look in the
     following locations in order:
     1. the current working directory.
     2. a subdirectory called shaders within the current working directory.
     3. the installed shaders directory INSTALL_PREFIX/share/MinGfx-1.0/shaders.
     4. the shaders directory in the MinGfx build tree.
     If the file is found, the full path is returned, else basename is returned.
     */
    static std::string FindMinGfxShaderFile(const std::string &basename);
    
private:
};


} // end namespace

#endif
