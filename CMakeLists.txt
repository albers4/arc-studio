cmake_minimum_required( VERSION 3.5 )

project( arc-studio )

find_package( OpenGL REQUIRED )

include_directories( ${OPENGL_INCLUDE_DIRS} )

set( GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL "GLFW lib only" )
set( GLAD_GL "" )

add_subdirectory( glfw-3.4 )

option( "USE-GLFW-GLAD" "Use GLAD from GLFW" ON )

if( USE-GLFW-GLAD )
    include_directories("${GLFW_SOURCE_DIR}/deps")
    set( GLAD_GL "${GLFW_SOURCE_DIR}/deps/glad/gl.h" )
endif()

set( ARC-STUDIO-SRC main.cpp )

add_executable( arc-studio ${ARC-STUDIO-SRC} ${GLAD_GL} )
target_link_libraries( arc-studio ${OPENGL_LIBRARIES} glfw )

set_target_properties( arc-studio PROPERTIES LINKER_LANGUAGE CXX )
