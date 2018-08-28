# Original Author(s) of this File: 
#   Daniel Keefe, 2017, University of Minnesota
#  
# Author(s) of Significant Updates/Modifications to the File:
#   ... 



# You are encouraged to copy this example, move it outside of the MinGfx directory, and use
# it as a starting point for your project.  When you do this, you'll have to edit the
# following line as needed to point to the MinGfx install prefix used on your system.

# !!!!!!!!!!!!! EDIT THE FOLLOWING LINE AS NEEDED !!!!!!!!!!!!! 
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../build/install ../../..)

# For example, you might have installed to a build tree that now looks like this:
# /Users/keefe/courses/cs4611/sw/include/MinGfx-1.0/
# /Users/keefe/courses/cs4611/sw/include/Eigen
# /Users/keefe/courses/cs4611/sw/include/nanogui
# /Users/keefe/courses/cs4611/sw/include/ etc.. 

# /Users/keefe/courses/cs4611/sw/lib/MinGfx-1.0/libMinGfx.a
# /Users/keefe/courses/cs4611/sw/lib/libnanogui.so
# /Users/keefe/courses/cs4611/sw/lib/ etc.. 

# If so, you would set the prefix path as follows:
#list(APPEND CMAKE_PREFIX_PATH /Users/keefe/courses/cs4611/sw)





#### BASIC PROJECT SETUP ####

project(mingfx-example)

# Using 3.9 to get a modern version of FindOpenGL.cmake
cmake_minimum_required (VERSION 3.9) 

# Dependencies that are auto-downloaded, built, and installed for you will go in the 
# directory pointed to by the CMAKE_INSTALL_PREFIX.  It defaults to a location inside
# the build directory.
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR "${CMAKE_INSTALL_PREFIX}" STREQUAL "")
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
endif()

# Add to paths cmake uses to search for scripts, modules, and config packages
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_INSTALL_PREFIX})
list(INSERT CMAKE_PREFIX_PATH 0 ${CMAKE_INSTALL_PREFIX})

include(MessageMacros)
h1("Building ${PROJECT_NAME}")
h2("Configuring paths")

message(STATUS "Module path: ${CMAKE_MODULE_PATH}")
message(STATUS "Prefix path: ${CMAKE_PREFIX_PATH}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")



#### SOURCE FOR THIS PROJECT ####
h2("Configuring source files")

set(SOURCEFILES
  main-circleviewer.cpp
  CircleViewer.cpp
)

set(HEADERFILES
  CircleViewer.h
)

set(EXTRAFILES
#  README.md
)

set_source_files_properties(${EXTRAFILES} PROPERTIES HEADER_FILE_ONLY TRUE)



#### COMPILE OPTIONS ####

h2("Configuring Compiler Options")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")


message(STATUS "Building for " ${CMAKE_SYSTEM_NAME} ".")

# Linux specific
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DLINUX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()


# Apple specific
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  add_definitions(-DOSX)

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

endif()


# Windows specific
if (WIN32)
  add_definitions(-DWIN32)
endif()




#### DEFINE TARGET(S) ####

h2("Defining Target(s)")

add_executable(${PROJECT_NAME} ${SOURCEFILES} ${HEADERFILES} ${EXTRAFILES})



#### FIND AND ADD DEPENDENCIES ####

h2("Adding Dependencies")
set(EXTERNAL_DIR external)


# MinGfx (linked with an imported cmake target so no need to specify include dirs)
# This will try to find MinGfxConfig.cmake, which should have been installed under
# CMAKE_INSTALL_PREFIX/lib/cmake/MinGfx when you installed the MinGfx Toolkit.
find_package(MinGfx REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC MinGfx::MinGfx)

# Add external dependency on OpenGL
include(AutoBuildOpenGL)
AutoBuild_use_package_OpenGL(${PROJECT_NAME} PUBLIC)



#### INSTALL TARGET(S) ####

h2("Configuring Install Target")

# The install locations are relative to the CMAKE_INSTALL_PREFIX variable
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

