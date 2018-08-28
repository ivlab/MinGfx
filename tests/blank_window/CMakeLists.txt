# This file is part of the MinGfx cmake build system.  
# See the main MinGfx/CMakeLists.txt file for details.

project(mingfx-test-blank-window)


# Source:
set (SOURCEFILES 
  main.cc
)
set (HEADERFILES
)
set (CONFIGFILES
)


# Define the target
add_executable(${PROJECT_NAME} ${HEADERFILES} ${SOURCEFILES})


# Add dependency on libMinGfx:
target_include_directories(${PROJECT_NAME} PUBLIC ../../src)
target_link_libraries(${PROJECT_NAME} PUBLIC MinGfx)

# Add external dependency on NanoGUI
include(AutoBuildNanoGUI)
AutoBuild_use_package_NanoGUI(${PROJECT_NAME} PUBLIC)



# Installation:
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${INSTALL_BIN_DEST}
        COMPONENT Tests)


# For better organization when using an IDE with folder structures:
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "Tests")
source_group("Header Files" FILES ${HEADERFILES})
set_source_files_properties(${CONFIGFILES} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("Config Files" FILES ${CONFIGFILES})

