# This file is part of the MinGfx cmake build system.  
# See the main MinGfx/CMakeLists.txt file for authors, copyright, and license info.


# Calling CMAKE_CURRENT_LIST_DIR inside a function returns the list dir of the calling script
# but we want the list dir of this file in order to find the DownloadHelper.txt.in file, which
# should be stored right next to this one.  So, defining this variable outside the scope of the
# functions below.
set(DIR_OF_THIS_FILE ${CMAKE_CURRENT_LIST_DIR})  



# Usage:  
# ExternalProject_Download(
#     # This first argument is the name of the project to download.  It is required:
#     glm
#
#     # Additional arguments specify how to download the project using GIT, SVN, CVS, or URL.
#     # These can be any of the arguments used for the downloading step of the cmake builtin
#     # ExternalProject_Add command. 
#     GIT_REPOSITORY "https://github.com/g-truc/glm.git"
#     GIT_TAG master
#     etc..
# )
function(ExternalProject_Download EXT_PROJECT_NAME DOWNLOAD_DIR)

    include(MessageMacros)
    h1("BEGIN EXTERNAL PROJECT DOWNLOAD (${EXT_PROJECT_NAME}).")

    h2("Creating a download helper project for ${EXT_PROJECT_NAME}.")

    set(DOWNLOAD_OPTIONS ${ARGN})
    string (REGEX REPLACE "(^|[^\\\\]);" "\\1 " DOWNLOAD_OPTIONS "${DOWNLOAD_OPTIONS}")


    file(MAKE_DIRECTORY ${DOWNLOAD_DIR}/${EXT_PROJECT_NAME})
    configure_file(
        ${DIR_OF_THIS_FILE}/DownloadHelper.txt.in 
        ${DOWNLOAD_DIR}/${EXT_PROJECT_NAME}/download-helper/CMakeLists.txt
    )

    h2("Generating build files for the ${EXT_PROJECT_NAME} download helper project.")
    execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" . WORKING_DIRECTORY "${DOWNLOAD_DIR}/${EXT_PROJECT_NAME}/download-helper")

    h2("Building the ${EXT_PROJECT_NAME} download helper project.  (This actually performs the download and may take some time...)")
    execute_process(COMMAND "${CMAKE_COMMAND}" --build . WORKING_DIRECTORY "${DOWNLOAD_DIR}/${EXT_PROJECT_NAME}/download-helper")

    h2("Completed download of external project ${EXT_PROJECT_NAME}.")

endfunction()


# Usage:  
# ExternalProject_BuildAndInstallNow(
#     # This first argument is the name of the external project to download.  It is required:
#     VRPN
#     # This second argument is the relative path from ${EXTERNAL_DIR_NAME}/projectname/ to the project's
#     # main CMakeLists.txt file:
#     src
#
#     # Additional arguments are passed on as options to the cmake build file generator
#     -DVRPN_BUILD_DIRECTSHOW_VIDEO_SERVER=OFF
#     -DVRPN_BUILD_HID_GUI=OFF
#     etc..
# )
function(ExternalProject_BuildAndInstallNow EXT_PROJECT_NAME DOWNLOAD_DIR RELPATH_TO_CMAKELISTS)

    include(MessageMacros)
    h1("BEGIN EXTERNAL PROJECT BUILD AND INSTALL (${EXT_PROJECT_NAME}).")

    # any extra args to the function are interpreted as arguments for the cmake config process
    set(CMAKE_CONFIG_OPTIONS ${ARGN})

    # always set the install prefix to be the same as for the main project
    list(APPEND CMAKE_CONFIG_OPTIONS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})

    #string (REGEX REPLACE "(^|[^\\\\]);" "\\1 " CMAKE_CONFIG_OPTIONS "${CMAKE_CONFIG_OPTIONS}")


    set(SRC_DIR "${DOWNLOAD_DIR}/${EXT_PROJECT_NAME}/${RELPATH_TO_CMAKELISTS}")
    set(BUILD_DIR "${CMAKE_BINARY_DIR}/external/${EXT_PROJECT_NAME}")

    file(MAKE_DIRECTORY ${BUILD_DIR})

    h2("Generating build files for external project ${EXT_PROJECT_NAME}.")
    message(STATUS "Using source dir: ${SRC_DIR}")
    message(STATUS "Using build dir: ${BUILD_DIR}")
    message(STATUS "Config options: ${CMAKE_CONFIG_OPTIONS}")

    execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" ${SRC_DIR} ${CMAKE_CONFIG_OPTIONS} WORKING_DIRECTORY ${BUILD_DIR})

    h2("Building external project ${EXT_PROJECT_NAME}.  (This may take some time...)")
    execute_process(COMMAND "${CMAKE_COMMAND}" --build ${BUILD_DIR} --target install)

    h2("Completed external build of ${EXT_PROJECT_NAME}.")

endfunction()

