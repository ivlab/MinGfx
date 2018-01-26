# This file is part of the MinGfx cmake build system.  
# See the main MinGfx/CMakeLists.txt file for authors, copyright, and license info.


# Usage: In your CMakeLists.txt, somewhere after you define the target that depends
# on the NanoGUI library (typical with something like add_executable(${PROJECT_NAME} ...) 
# or add_library(${PROJECT_NAME} ...)), add the following two lines:

#    include(UseNanoGUI)
#    UseNanoGUI(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external)

# The second argument can be either PUBLIC, PRIVATE, or INTERFACE, following the keyword
# usage described here: 
# https://cmake.org/cmake/help/latest/command/target_include_directories.html

# The third argument is the directory to use for downloading the external project if
# autobuild is used.




macro(UseNanoGUI YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE DOWNLOAD_DIR)

    option(AUTOBUILD_NANOGUI "If cmake cannot find NanoGUI pre-installed on the system and you turn this option ON, the next time you run the cmake configure step it will: 1. download NanoGUI, 2. build NanoGUI, and 3. install NanoGUI to the current CMAKE_INSTALL_PREFIX.  Then, your main project should be able to find and link with NanoGUI." OFF)


    message(STATUS "Searching for NanoGUI library...")

    # Check to see if the library is already installed on the system

    # This uses our own FindNanoGUI.cmake script
    find_package(NanoGUI)

    # Case 1: Already installed on the system
    if (${NANOGUI_FOUND})

        message(STATUS "Ok: NanoGUI Found.")
        message(STATUS "NanoGUI headers: ${NANOGUI_INCLUDE_DIR}")
        message(STATUS "NanoGUI libs: ${NANOGUI_LIBRARIES}")

    # Case 2: Download, build and install it now for the user, then try find_package() again
    elseif (AUTOBUILD_NANOGUI)

        set(NANOGUI_AUTOBUILT TRUE)

        message(STATUS "Ok: AUTOBUILD_NANOGUI is ON so NanoGUI will be downloaded and built for you.")

        include(ExternalProjectDownloadBuildInstall)
        ExternalProject_Download(
            NanoGUI
            ${DOWNLOAD_DIR}
            GIT_REPOSITORY https://github.com/wjakob/nanogui.git
        )

        ExternalProject_BuildAndInstallNow(
            NanoGUI
            ${DOWNLOAD_DIR}
            src
            -DNANOGUI_BUILD_PYTHON=OFF
        )


        # NanoGUI's install doesn't seem to install its dependencies, so do it manually here.
        file(INSTALL ${DOWNLOAD_DIR}/NanoGUI/src/ext/eigen/Eigen DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

        file(INSTALL ${DOWNLOAD_DIR}/NanoGUI/src/ext/glfw/include/GLFW DESTINATION ${CMAKE_INSTALL_PREFIX}/include    
            FILES_MATCHING PATTERN glfw3.h PATTERN glfw3native.h)

        file(INSTALL ${DOWNLOAD_DIR}/NanoGUI/src/ext/nanovg/src/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/nanovg    
            FILES_MATCHING PATTERN "*.h")

        if (WIN32)
            # And, the don't even install the .dll on Windows!  Come on nanogui guys, get your act together!
            file(INSTALL ${CMAKE_BINARY_DIR}/external/NanoGUI/Debug/nanogui.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin OPTIONAL)
            file(INSTALL ${CMAKE_BINARY_DIR}/external/NanoGUI/Release/nanogui.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin OPTIONAL)
            file(INSTALL ${DOWNLOAD_DIR}/NanoGUI/src/ext/glad/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
        endif()

        # Try find_package() again
        message(STATUS "Searching (again, right after autobuilding) for NanoGUI library...")
        find_package(NanoGUI)
 
        # We just intalled it to CMAKE_INSTALL_PREFIX, and the root CMakeLists.txt puts this in the
        # CMAKE_MODULE_PATH.  So, if we were not able to find the package now, then something is very wrong.
        if (NOT ${NANOGUI_FOUND})
            message(FATAL_ERROR "MinVR did an autobuild of the NanoGUI dependency, and it should now be installed at the prefix ${CMAKE_INSTALL_PREFIX}, but cmake is still unable to find it with find_package().")
        endif()

        message(STATUS "NanoGUI headers: ${NANOGUI_INCLUDE_DIRS}")
        message(STATUS "NanoGUI libs: ${NANOGUI_LIBRARIES}")


    # Case 3: Error out when not found and explain how to use AUTOBUILD.
    else()

        message(FATAL_ERROR "The NanoGUI library was not found on the system.  You can: (1) install NanoGUI yourself, (2) point cmake to an already-installed version of NanoGUI by adding the installation prefix of NanoGUI to the CMAKE_PREFIX_PATH environment variable, or (3) set AUTOBUILD_NANOGUI to ON and run cmake configure again.  AUTOBUILD_NANOGUI will: 1. download NanoGUI, 2. build NanoGUI, and 3. install NanoGUI to the CMAKE_INSTALL_PREFIX (currently set to ${CMAKE_INSTALL_PREFIX}).  If you want to use the AUTOBUILD feature, adjust CMAKE_INSTALL_PREFIX as desired now, set AUTOBUILD_NANOGUI to ON, and then run cmake configure again.")

    endif()


    # If we reach this point without an error, then one of the calls to find_package() was successful

    message(STATUS "Linking target ${YOUR_TARGET} with ${INTERFACE_PUBLIC_OR_PRIVATE} dependency NanoGUI.")

    target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${NANOGUI_LIBRARIES})
    target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${NANOGUI_INCLUDE_DIRS})
    
    target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_NANOGUI)

    if (WIN32)
        target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DNANOGUI_GLAD -DGLAD_GLAPI_EXPORT)
    endif()

endmacro()

