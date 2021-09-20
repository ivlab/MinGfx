# This file is part of the MinGfx cmake build system.  
# See the main MinGfx/CMakeLists.txt file for details.


set(HEADERFILES
    src/aabb.h
    src/bvh.h
    src/color.h
    src/craft_cam.h
    src/default_shader.h
    src/gfxmath.h
    src/graphics_app.h
    src/matrix4.h
    src/mesh.h
    src/mingfx.h
    src/mingfx_config.h
    src/opengl_headers.h
    src/platform.h
    src/point2.h
    src/point3.h
    src/quaternion.h
    src/quick_shapes.h
    src/ray.h
    src/shader_program.h
    src/text_shader.h
    src/texture2d.h
    src/unicam.h
    src/vector2.h
    src/vector3.h
)

set(SOURCEFILES
    src/aabb.cc
    src/bvh.cc
    src/color.cc
    src/craft_cam.cc
    src/default_shader.cc
    src/gfxmath.cc
    src/graphics_app.cc
    src/matrix4.cc
    src/mesh.cc
    src/platform.cc
    src/point2.cc
    src/point3.cc
    src/quaternion.cc
    src/quick_shapes.cc
    src/ray.cc
    src/shader_program.cc
    src/text_shader.cc
    src/texture2d.cc
    src/unicam.cc
    src/vector2.cc
    src/vector3.cc
)

set(EXTRAFILES
    src/mingfx_config.h.in
)

set(SHADERFILES
    src/shaders/default.frag
    src/shaders/default.vert
    src/shaders/fullscreen.frag
    src/shaders/fullscreen.vert
    src/shaders/text.frag
    src/shaders/text.vert
)

set_source_files_properties(${EXTRAFILES} PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties(${SHADERFILES} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group("Header Files" FILES ${HEADERFILES})
source_group("Source Files" FILES ${SOURCEFILES})
source_group("Shaders" FILES ${SHADERFILES})

#add_library(MinGfx SHARED ${HEADERFILES} ${SOURCEFILES} ${EXTRAFILES} ${SHADERFILES})

add_library(MinGfx ${HEADERFILES} ${SOURCEFILES} ${EXTRAFILES} ${SHADERFILES})


# Using target_include_directories() rather than just include_directories() is
# critical in order to support generating a MinGfxConfig.cmake file.  It supports
# generator expressions, so we can point to two different include dirs depending
# on whether building or using the installed version.
target_include_directories(MinGfx PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>  # for headers when building
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DEST}>        # for client in install mode
)

# Add external dependency on NanoGUI
include(AutoBuildNanoGUI)
AutoBuild_use_package_NanoGUI(MinGfx PUBLIC)

# Add external dependency on OpenGL
include(AutoBuildOpenGL)
AutoBuild_use_package_OpenGL(MinGfx PUBLIC)


install(TARGETS MinGfx EXPORT MinGfxTargets COMPONENT CoreLib
  LIBRARY DESTINATION "${INSTALL_LIB_DEST}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DEST}"
  RUNTIME DESTINATION "${INSTALL_BIN_DEST}"
)

install(FILES ${HEADERFILES} DESTINATION "${INSTALL_INCLUDE_DEST}" COMPONENT CoreLib)
install(FILES ${SHADERFILES} DESTINATION "${INSTALL_SHADERS_DEST}" COMPONENT CoreLib)

