# This file is part of the MinGfx cmake build system.  
# See the main MinGfx/CMakeLists.txt file for details.


set(HEADERFILES
    src/color.h
    src/graphics_app.h
    src/matrix4.h
    src/mingfx.h
    src/opengl_headers.h
    src/platform.h
    src/point3.h
    src/quick_shapes.h
    src/texture2d.h
    src/vector3.h
)

set(SOURCEFILES
    src/color.cc
    src/graphics_app.cc
    src/matrix4.cc
    src/platform.cc
    src/point3.cc
    src/quick_shapes.cc
    src/texture2d.cc
    src/vector3.cc
)

source_group("Header Files" FILES ${HEADERFILES})
source_group("Source Files" FILES ${SOURCEFILES})

add_library(MinGfx SHARED ${HEADERFILES} ${SOURCEFILES})


# Using target_include_directories() rather than just include_directories() is
# critical in order to support generating a MinGfxConfig.cmake file.  It supports
# generator expressions, so we can point to two different include dirs depending
# on whether building or using the installed version.
target_include_directories(MinGfx PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>  # for headers when building
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DEST}>        # for client in install mode
)

# Add dependency on NanoGUI
include(UseNanoGUI)
UseNanoGUI(MinGfx PUBLIC ${EXTERNAL_DIR})

# Add dependency on OpenGL
include(UseOpenGL)
UseOpenGL(MinGfx PUBLIC ${EXTERNAL_DIR})



install(TARGETS MinGfx EXPORT MinGfxTargets COMPONENT CoreLib
  LIBRARY DESTINATION ${INSTALL_LIB_DEST}
  ARCHIVE DESTINATION ${INSTALL_LIB_DEST}
  RUNTIME DESTINATION ${INSTALL_BIN_DEST}
)

install(FILES ${HEADERFILES} DESTINATION ${INSTALL_INCLUDE_DEST} COMPONENT CoreLib)
