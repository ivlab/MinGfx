# Copyright (c) 2017,2018 Regents of the University of Minnesota.
# All rights reserved.
#
# Original Author(s) of this File: 
#   Daniel Keefe, Jan 2018, University of Minnesota -- This build system is inspired by
#   the AUTOBUILD_DEPENDENCY approach I developed for the MinVR Open Source project.  
#   Tom Sgouros (Brown Univ.) and Dan Orban (Univ. of Minn.) also contributed to earlier 
#   versions of MinVR's build system and the use of cmake in other research software used
#   in the IV/LAB.  So, this is also at least loosly inspired by their contributions.  
#   See minvr.org for a full list of contributors to that project.
#  
# Author(s) of Significant Updates/Modifications to the File:
#   ...




#### BASIC PROJECT SETUP ####

# Depends on cmake 3.9+ for its more advanced FindOpenGL.cmake module
cmake_minimum_required (VERSION 3.9)

# Search path for cmake support scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

# Version is <major>.<minor>.<patch>
cmake_policy(SET CMP0048 NEW)  # Required for setting version number as part of the project() command
project(MinGfx_Toolkit VERSION 1.0.0)

# This will be appended to the install destinations so that multiple versions of MinGfx
# can be safely installed on the same system.  This intentionally does not include a
# patch number because patches should be small changes that do not change the API.
set(MINGFX_VERSION_STR -${MinGfx_Toolkit_VERSION_MAJOR}.${MinGfx_Toolkit_VERSION_MINOR})

# Organize folder structure for Xcode, Visual Studio, etc.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# For better formatting of output while configuring with cmake
include(MessageMacros)

# If you "autobuild" external dependencies as part of the cmake configure process, then they will be
# downloaded to this directory.
set(AUTOBUILD_DOWNLOAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external" CACHE STRING "Location for external project sources downloaded by AutoBuild.")


set(CMAKE_INSTALL_MESSAGE ALWAYS)

include(AutoBuild)
AutoBuild_init()


#### ADJUST NAMES OF OUTPUT FILES BASED ON BUILD TYPE ####

h1("CONFIGURING BUILD TYPES")

# To distinguish between debugging, release, and other builds, we'll
# add a postfix to the name of the lib or exe that we generate.

set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "rd")
set(CMAKE_MINSIZEREL_POSTFIX "s")

if (CMAKE_BUILD_TYPE MATCHES "Release")
  set(CMAKE_BUILD_POSTFIX "${CMAKE_RELEASE_POSTFIX}")
elseif (CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
  set(CMAKE_BUILD_POSTFIX "${CMAKE_MINSIZEREL_POSTFIX}")
elseif (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
  set(CMAKE_BUILD_POSTFIX "${CMAKE_RELWITHDEBINFO_POSTFIX}")
elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
  set(CMAKE_BUILD_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
else()
  set(CMAKE_BUILD_POSTFIX "")
endif()




#### SETUP DIRECTORIES FOR BUILDING AND INSTALLING ####

h1("CONFIGURING BUILD/INSTALL LOCATIONS")

h2("Configuring build directories.")

message(STATUS "Tests, other binaries: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "Libraries: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")


h2("Configuring installation directories.")
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
endif()

# When we autobuild dependencies, we install them in CMAKE_INSTALL_PREFIX, and then look for them
# there using find_package().  So, this makes sure the install prefix is in the path that 
# find_package() searches.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_INSTALL_PREFIX})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX})

# These are relative to ${CMAKE_INSTALL_PREFIX}
set(INSTALL_BIN_DEST           bin)
set(INSTALL_LIB_DEST           lib/MinGfx${MINGFX_VERSION_STR})
set(INSTALL_INCLUDE_DEST       include/MinGfx${MINGFX_VERSION_STR})
set(INSTALL_SHADERS_DEST       share/MinGfx${MINGFX_VERSION_STR}/shaders)
set(INSTALL_DATA_DEST          share/MinGfx${MINGFX_VERSION_STR}/data)
set(INSTALL_DOC_DEST           share/MinGfx${MINGFX_VERSION_STR}/docs)
set(INSTALL_CMAKECONFIG_DEST   lib/cmake/MinGfx${MINGFX_VERSION_STR})

message(STATUS "Tests, other binaries: ${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN_DEST}")
message(STATUS "MinGfx library: ${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DEST}")
message(STATUS "MinGfx header files: ${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DEST}")
message(STATUS "MinGfx shader programs: ${CMAKE_INSTALL_PREFIX}/${INSTALL_SHADERS_DEST}")
message(STATUS "MinGfx data files: ${CMAKE_INSTALL_PREFIX}/${INSTALL_DATA_DEST}")
message(STATUS "MinGfx documentation files: ${CMAKE_INSTALL_PREFIX}/${INSTALL_DOC_DEST}")
message(STATUS "CMake Config Package for including MinGfx in other projects: ${CMAKE_INSTALL_PREFIX}/${INSTALL_CMAKECONFIG_DEST}")

# Configure a header file to pass some of the CMake settings to the source code
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mingfx_config.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mingfx_config.h
)


#### SET ANY COMPILER FLAGS ####

h1("CONFIGURING COMPLIER FLAGS")



message(STATUS "Building for " ${CMAKE_SYSTEM_NAME} ".")
message(STATUS "Compiler supported features = ${CMAKE_CXX_COMPILE_FEATURES}")


# Linux specific
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DLINUX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()


# Apple specific
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  add_definitions(-DOSX)

  # Since for some stupid reason Apple has now deprecated OpenGL
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
  add_definitions(-DGL_SILENCE_DEPRECATION)

  # RPATH settings, see https://cmake.org/Wiki/CMake_RPATH_handling
  set(CMAKE_MACOSX_RPATH ON)

  # use, i.e. don't skip the full RPATH for the build tree
  SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

  # when building, don't use the install RPATH already
  # (but later on when installing)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

  SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

  # add the automatically determined parts of the RPATH
  # which point to directories outside the build tree to the install RPATH
  SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

  # the RPATH to be used when installing, but only if it's not a system directory
  LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
  IF("${isSystemDir}" STREQUAL "-1")
     SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  ENDIF("${isSystemDir}" STREQUAL "-1")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()


# Windows specific
if (WIN32)
  add_definitions(-DWIN32)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Z7")
endif()




#### DEFINE THE LIBRARY WE WANT TO BUILD ####

h1("CONFIGURING LIBMINGFX")

h2("Configuring core library source.")
message(STATUS "Adding src to the build.")

# Intentionally using include() rather than add_subdirectory() so that the MinGfx target
# is defined in the scope of this CMakeLists.txt file.  That way, sudirs that depend
# upon it can list it as a dependency.
include(src/CMakeLists.txt)




#### DEFINE ADDITIONAL BUILD TARGETS, LIKE TEST PROGRAMS, ETC. ####

h1("CONFIGURING PROGRAMS AND DATA")

h2("Cofiguring test programs.")
message(STATUS "Adding all subirectories of tests to the build.")
add_subdirectory(tests/blank_window)
add_subdirectory(tests/gui_plus_opengl)


h2("Cofiguring data.")
message(STATUS "Identifying data to install.")
install(
    DIRECTORY data/
    DESTINATION ${INSTALL_DATA_DEST}
)



#### OPTIONALLY, BUILD DOXYGEN-STYLE DOCUMENTATION ####

h1("CONFIGURING DOCUMENTATION")
  
h2("Configuring Web-Based Documentation.")
option(WITH_DOCUMENTATION "Builds web-based documentation for the library. (Requires doxygen)")
if (WITH_DOCUMENTATION)
  message(STATUS "ON: Will build and install MinGfx Web-Based Documentation.")
else()
  message(STATUS "OFF: NOT building MinGfx Web-Based Documentation.")
endif()

if (WITH_DOCUMENTATION)
    find_package(Doxygen)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(DOXY_FILES
        doc/Doxyfile.in
        doc/api.md
        doc/installation.md
        doc/mainpage.md
        doc/header.html
        doc/index.html
        doc/footer.html
        doc/customdoxygen.css
    )

    set(OTHER_DOCS
        README.md
        LICENSE.txt
    )

    # Process the config file and put it in the build directory.
    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYFILE_IN} ${DOXYFILE} @ONLY)

    add_custom_target(doc ALL
        ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
        DEPENDS ${DOXY_FILES} ${OTHER_DOCS}
        SOURCES ${DOXY_FILES} ${OTHER_DOCS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation with Doxygen" 
        VERBATIM
    )

    set_source_files_properties(${DOXY_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
    source_group("Doxygen Files" FILES ${DOXY_FILES})

    set_source_files_properties(${OTHER_DOCS} PROPERTIES HEADER_FILE_ONLY TRUE)
    source_group("Other Docs" FILES ${OTHER_DOCS})

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html 
        DESTINATION ${INSTALL_DOC_DEST}
        COMPONENT Docs
        OPTIONAL
    )

    install(FILES doc/index.html
        DESTINATION ${INSTALL_DOC_DEST}
        COMPONENT Docs
        OPTIONAL
    )
endif()





#### CREATE/INSTALL A CMAKE CONFIG PACKAGE TO HELP OTHERS LINK TO THE LIBRARY ####

h1("WRITING CMAKE CONFIG PACKAGE")

# The documentation for setting this up is so bad.  These pages all helped.  My current
# understanding (dfk) is summarized below to help others.
# https://cmake.org/cmake/help/git-master/manual/cmake-packages.7.html#creating-packages
# https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#target-usage-requirements
# http://foonathan.net/blog/2016/03/03/cmake-install.html
# https://archive.fosdem.org/2013/schedule/event/moderncmake/attachments/slides/258/export/events/attachments/moderncmake/slides/258/cmake_fosdem_2013.pdf

# CMake Package Config in a Nutshell:

# CMake config packages are the newer, recommended way of distributing projects built with CMake.
# They replace the older FindModule.cmake approach for including dependencies in a cmake project, 
# but not everyone has yet switched over to the new approach.  Both styles can work together, and
# this seems to often be required because many libraries are not yet distributing a *Config.cmake
# file with their install tree.


# 1. The Difference Between "Find Modules" (Find*.cmake) and "Config Packages" (*Config.cmake)

# Whereas, Find*.cmake files are either installed on the system or written by downstream projects.
# *Config.cmake files are created and installed by the upstream projects (e.g., we want generate
# MinVRConfig.cmake and install it whenever the MinVR project in installed).

# When you use a Find Module, it defines several variables (e.g., OPENGL_INCLUDE_DIR, OPENGL_LIBRARIES)
# that you must then be sure to add to the include dirs and libs to link with your project.
# Config Packages are different.  Instead, they "export" the targets defined in the original project
# (e.g., the target MinGfx is used in this file to build libMinGfx.a) so that the client project
# may refer to these targets directly.  This is pretty cool because we can define the MinGfx target
# in such a way that cmake even knows which header files are associated with it, and will add the
# required include flags whenever the MinGfx target is used.


# 2. How to use Config Packages in a client CMakeLists.txt 

# With the config package appraoch, client projects only need to include two lines to use our library:

# find_package(MinGfx REQUIRED)
# ... then, after add_executable(${PROJECT_NAME}) or add_library(${PROJECT_NAME})
# target_link_libraries(${PROJECT_NAME} PUBLIC MinGfx)


# 3. Finding *Config.cmake files

# Config packages are files of the form MyProjNameConfig.cmake (or *-config.cmake).  Each library
# provides and installs its own config package.  So, here we will create one to install with MinGfx.
# If created correctly, these packages can be relocatable, i.e., not specific to the install path
# used on a particular machine.  When you call find_package(...) cmake looks for packages in a
# variety of locations (e.g., /usr/local/lib/cmake/*).  

# Since config packages are supposed to be written by and installed by the upstream dependencies,
# my understanding is that you should not attempt to write you own version of a config package if
# you encounter a project that is not (yet) using the config package appraoch.  Instead, use the
# older Find Module appraoch.  If CMake ships with a Find*.cmake file for that project, then use 
# it.  If not, then you can write your own.  Since making this switch, many of the newer
# Find*.cmake scripts that ship with CMake have been updated to support imported targets. They
# are still called something like FindOpenGL.cmake, but in addition to just setting vars like
# OPENGL_INCLUDE_DIR, they also define targets, such as OpenGL::GL.  We use these when we can
# because they are more robust.  When you do target_link_libraries(myproj PULBIC OpenGL::GL)
# cmake knows to not only link with the the opengl library but also to include its headers and
# link with all dependencies.


# This tells cmake to generate a MinGfxTargets.cmake file that works for importing all targets
# associated with the MinGfx export group from the install tree.  This file is where all the
# important cmake magic happens. It is auto-generated.  This call also installs the file to 
# the appropriate location.
install(EXPORT MinGfxTargets
  NAMESPACE MinGfx::
  FILE MinGfxTargets.cmake
  DESTINATION ${INSTALL_CMAKECONFIG_DEST}
)

# This is a very small wrapper around the MinGfxTargets.cmake file.  It's possible to use this
# configure_file() command to replace variables (e.g., special defines, install paths) in the 
# file, but we do not currently need that functionality.
configure_file(
  cmake/MinGfxConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/MinGfxConfig.cmake"
  COPYONLY
)

# This auto-generates a standard CMake config version file. Using this, client projects can
# check to see if a specific version of MinGfx is installed on the system.
include(CMakePackageConfigHelpers)
set_property(TARGET MinGfx PROPERTY VERSION ${MinGfx_Toolkit_VERSION})
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/MinGfxConfigVersion.cmake"
  VERSION ${MinGfx_Toolkit_VERSION}
  COMPATIBILITY AnyNewerVersion
)

# The two Config files also get installed
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/MinGfxConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/MinGfxConfigVersion.cmake"
  DESTINATION
    "${INSTALL_CMAKECONFIG_DEST}"
)


AutoBuild_check_status()

