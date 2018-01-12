

find_path(NANOGUI_INCLUDE_DIR 
    nanogui/nanogui.h
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/include 
    ${CMAKE_INSTALL_PREFIX}/include/nanogui
    $ENV{NANOGUI_ROOT}/include 
    $ENV{NANOGUI_ROOT}/include/nanogui 
    /usr/local/include
    /usr/local/include/nanogui
)


find_library(NANOGUI_OPT_LIBRARIES 
  NAMES 
    nanogui
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/lib 
    $ENV{NANOGUI_ROOT}/lib 
    /usr/local/lib
)

          
find_library(NANOGUI_DEBUG_LIBRARIES 
  NAMES 
    nanoguid
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/lib 
    $ENV{NANOGUI_ROOT}/lib 
    /usr/local/lib
)

set(NANOGUI_INCLUDE_DIRS
    ${NANOGUI_INCLUDE_DIR}
    ${NANOGUI_INCLUDE_DIR}/nanovg
)

unset(NANOGUI_LIBRARIES)
if (NANOGUI_OPT_LIBRARIES)
  list(APPEND NANOGUI_LIBRARIES ${NANOGUI_OPT_LIBRARIES})
endif()

if (NANOGUI_DEBUG_LIBRARIES)
  list(APPEND NANOGUI_LIBRARIES ${NANOGUI_DEBUG_LIBRARIES})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    NANOGUI
    DEFAULT_MSG
    NANOGUI_INCLUDE_DIRS
    NANOGUI_LIBRARIES
)

mark_as_advanced(
    NANOGUI_INCLUDE_DIR 
    NANOGUI_INCLUDE_DIRS 
    NANOGUI_OPT_LIBRARIES 
    NANOGUI_DEBUG_LIBRARIES 
    NANOGUI_LIBRARIES
)
