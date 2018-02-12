/*
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 See corresponding header file for details.
 */

#include "platform.h"

#include "mingfx_config.h"

#include <vector>
#include <sstream>

#ifdef WIN32
    #include <windows.h>
#else
    #include <sys/stat.h>
#endif


namespace mingfx {
    
bool Platform::FileExists(const std::string &filename) {
#ifdef WIN32
	LPCTSTR  szPath = (LPCTSTR)filename.c_str();
	DWORD dwAttrib = GetFileAttributes(szPath);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat buf;
    return (stat(filename.c_str(), &buf) == 0);
#endif
}
    

std::string Platform::FindFile(const std::string &basename, const std::vector<std::string> &searchpath) {
    for (int i=0; i<searchpath.size(); i++) {
        std::string fname = searchpath[i] + "/" + basename;
        if (Platform::FileExists(fname)) {
            return fname;
        }
    }
    return basename;
}

std::string Platform::FindFile(const std::string &basename, const std::string &searchpath) {
    std::vector<std::string> paths;
    std::stringstream ss(searchpath);
    std::string path;
    while (ss >> path) {
        paths.push_back(path);
        if (ss.peek() == ';')
            ss.ignore();
    }
    return FindFile(basename, paths);
}


std::string Platform::FindMinGfxDataFile(const std::string &basename) {
    std::vector<std::string> searchpath;
    searchpath.push_back(".");
    searchpath.push_back("data");
    searchpath.push_back(MINGFX_DATA_DIR_INSTALL);
    searchpath.push_back(MINGFX_DATA_DIR_BUILD);
    return FindFile(basename, searchpath);
}

std::string Platform::FindMinGfxShaderFile(const std::string &basename) {
    std::vector<std::string> searchpath;
    searchpath.push_back(".");
    searchpath.push_back("shaders");
    searchpath.push_back(MINGFX_SHADERS_DIR_INSTALL);
    searchpath.push_back(MINGFX_SHADERS_DIR_BUILD);
    return FindFile(basename, searchpath);
}


} // end namespace
