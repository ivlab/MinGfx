# Author: Daniel F. Keefe
# Copyright 2017,2018, Daniel F. Keefe and Regents of the University of Minnesota
# All Rights Reserved.

include(AutoBuild)


# Usage:
# AutoBuild_use_package_NanoGUI(
#    # The 1st argument is required.  It is the name of the target you wish to link this dependency to.
#    my-program 
#
#    # The 2nd argument is required.  It impacts how dependencies are propogated through CMake.  You can treat
#    # this dependency as an INTERFACE, PUBLIC, or PRIVATE.  See CMake's docs on Imported Targets for details.
#    PUBLIC 
# )
#
macro(AutoBuild_use_package_NanoGUI YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)
    set(PACKAGE_NAME "NanoGUI")
    string(TOUPPER ${PACKAGE_NAME} PACKAGE_NAME_UPPER)

    AutoBuild_find_package_module_mode(${PACKAGE_NAME})

    if ("${AUTOBUILD_PACKAGE_${PACKAGE_NAME_UPPER}}")
        if ("${AUTOBUILD_EXECUTE_NOW}")

            message(STATUS "AutoBuild: Beginning download, build, install sequence.")

            AutoBuild_download_project( 
                ${PACKAGE_NAME}
                GIT_REPOSITORY https://github.com/wjakob/nanogui.git
            )

            AutoBuild_build_and_install_project(
                ${PACKAGE_NAME}
                .
                -DNANOGUI_BUILD_PYTHON=OFF
            )


            # NanoGUI's install doesn't seem to install its dependencies, so do it manually here.
            file(INSTALL ${AUTOBUILD_DOWNLOAD_DIR}/NanoGUI/src/ext/eigen/Eigen DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

            file(INSTALL ${AUTOBUILD_DOWNLOAD_DIR}/NanoGUI/src/ext/glfw/include/GLFW DESTINATION ${CMAKE_INSTALL_PREFIX}/include    
                 FILES_MATCHING PATTERN glfw3.h PATTERN glfw3native.h)

            file(INSTALL ${AUTOBUILD_DOWNLOAD_DIR}/NanoGUI/src/ext/nanovg/src/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/nanovg    
                 FILES_MATCHING PATTERN "*.h")

            if (WIN32)
                # And, the don't even install the .dll on Windows!  Come on nanogui guys, get your act together!
                file(INSTALL ${CMAKE_BINARY_DIR}/external/NanoGUI/Debug/nanogui.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin OPTIONAL)
                #file(INSTALL ${CMAKE_BINARY_DIR}/external/NanoGUI/Release/nanogui.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/bin OPTIONAL)
                file(INSTALL ${AUTOBUILD_DOWNLOAD_DIR}/NanoGUI/src/ext/glad/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
            endif()


            AutoBuild_find_built_package_module_mode(${PACKAGE_NAME})

            set(${PACKAGE_NAME_UPPER}_AUTOBUILT "TRUE" CACHE BOOL "Confirms that package ${PACKAGE_NAME} was successfully built by the AutoBuild system.")
            mark_as_advanced(${PACKAGE_NAME}_AUTOBUILT)

        else()
            message(STATUS "AutoBuild: Scheduled to build ${PACKAGE_NAME} the next time CMake Configure is run.")
        endif()
    endif()


    # If we've found the package, either directly on the system or after autobuilding, then link with it
    if ("${${PACKAGE_NAME}_FOUND}" OR "${${PACKAGE_NAME_UPPER}_FOUND}")
        message(STATUS "Linking target ${YOUR_TARGET} with ${INTERFACE_PUBLIC_OR_PRIVATE} dependency ${PACKAGE_NAME}.")

        target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${NANOGUI_LIBRARIES})
        target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${NANOGUI_INCLUDE_DIRS})

        target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_${PACKAGE_NAME})

        if (WIN32)
            target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DNANOGUI_GLAD -DGLAD_GLAPI_EXPORT)
        endif()

    endif()

endmacro()

