cmake_minimum_required( VERSION 3.10 )
project( arc-studio VERSION 0.1.0 )

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0072 NEW)

# Find OpenGL
find_package( OpenGL REQUIRED )
include_directories( ${OPENGL_INCLUDE_DIRS} )

# GLFW configuration
set( GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL "GLFW lib only" )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW examples" )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW tests" )

if( UNIX AND NOT APPLE )
    set( GLFW_BUILD_WAYLAND OFF CACHE BOOL "Disable Wayland" )
    set( GLFW_BUILD_X11 ON CACHE BOOL "Enable X11" )
endif()

set( GLAD_GL "" )

# Check if glfw directory exists
if( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.4" )
    add_subdirectory( glfw-3.4 )
else()
    message( FATAL_ERROR "glfw-3.4 directory not found. Please clone with submodules or download GLFW." )
endif()

option( "USE-GLFW-GLAD" "Use GLAD from GLFW" ON )

if( USE-GLFW-GLAD )
    include_directories("${GLFW_SOURCE_DIR}/deps")
    set( GLAD_GL "${GLFW_SOURCE_DIR}/deps/glad/gl.h" )
endif()

# Source files
set( ARC-STUDIO-SRC main.cpp )

# Create executable
add_executable( arc-studio ${ARC-STUDIO-SRC} ${GLAD_GL} )

# Link libraries
target_link_libraries( arc-studio ${OPENGL_LIBRARIES} glfw )

# Platform-specific settings
if( WIN32 )
    set_target_properties(arc-studio PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    target_link_libraries(arc-studio ${OPENGL_LIBRARIES} glfw)
    set_target_properties(arc-studio PROPERTIES
        LINK_FLAGS "/ENTRY:mainCRTStartup"
    )
elseif( APPLE )
    set_target_properties(arc-studio PROPERTIES
        MACOSX_BUNDLE TRUE
    )
    target_link_libraries(arc-studio
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()

set_target_properties( arc-studio PROPERTIES LINKER_LANGUAGE CXX )

# Install rules
install(TARGETS arc-studio
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)