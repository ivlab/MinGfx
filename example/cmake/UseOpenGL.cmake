# This file is part of the MinGfx cmake build system.  
# See the main MinGfx/CMakeLists.txt file for authors, copyright, and license info.

# Either finds a pre-installed version or complains.

# Usage: In your CMakeLists.txt, somewhere after you define the target that depends
# on the OpenGL library (typical with something like add_executable(${PROJECT_NAME} ...) 
# or add_library(${PROJECT_NAME} ...)), add the following two lines:

#    include(UseOpenGL)
#    UseOpenGL(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external)

# The second argument can be either PUBLIC, PRIVATE, or INTERFACE, following the keyword
# usage described here: 
# https://cmake.org/cmake/help/latest/command/target_include_directories.html

# The third argument is the directory to use for downloading the external project if
# autobuild is used.



macro(UseOpenGL YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE DOWNLOAD_DIR)

    message(STATUS "Searching for OpenGL...")

	# Check to see if the library is already installed on the system    
    # CMake ships with FindOpenGL.cmake and in CMake 3.9+ it defines
    # the imported targets OpenGL::GL and OpenGL::GLU.  Using these is
    # now the preferred way to link with OpenGL and all of its dependencies.
	# See https://cmake.org/cmake/help/v3.9/module/FindOpenGL.html
	find_package(OpenGL)

	if (NOT ${OPENGL_FOUND})
	    message(FATAL_ERROR "OpenGL was not found on the system.  MinGfx can auto-download and build many dependencies for you, but not OpenGL. It should come pre-installed on your system.")
	endif()

    message(STATUS "Ok: OpenGL Found.")
    message(STATUS "OpenGL headers: ${OPENGL_INCLUDE_DIR}")
    message(STATUS "OpenGL libs: ${OPENGL_LIBRARIES}")


    message(STATUS "Linking target ${YOUR_TARGET} with ${INTERFACE_PUBLIC_OR_PRIVATE} dependency OpenGL::GL.")
    target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} OpenGL::GL)

    if (${OPENGL_GLU_FOUND})
        message(STATUS "Linking target ${YOUR_TARGET} with ${INTERFACE_PUBLIC_OR_PRIVATE} dependency OpenGL::GLU.")
        target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} OpenGL::GLU)
    endif()

	target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_OPENGL)

endmacro()
