cmake_minimum_required( VERSION 3.10 )
project( arc-studio VERSION 0.1.0 )

# C++ standard
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

# use cmake -DCMAKE_BUILD_TYPE=Release .. from within build dir
#if( CMAKE_CXX_COMPILER_ID MATCHES "GNU|CLANG" )
#    add_compile_options(-O3 -march=native -flto -DNDEBUG)
#    add_link_options(-flto)
#endif()

cmake_policy(SET CMP0072 NEW)

# Find OpenGL
find_package( OpenGL REQUIRED )

# GLFW configuration
set( GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL "GLFW lib only" )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW examples" )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW tests" )

if( UNIX AND NOT APPLE )
    set( GLFW_BUILD_WAYLAND OFF CACHE BOOL "Disable Wayland" )
    set( GLFW_BUILD_X11 ON CACHE BOOL "Enable X11" )
endif()

# Check if glfw directory exists
if( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.4" )
    add_subdirectory( glfw-3.4 )
else()
    message( FATAL_ERROR "glfw-3.4 directory not found. Please clone with submodules or download GLFW." )
endif()

# GLAD configuration
set( GLAD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/glad/include" )
set( GLAD_SRC "${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c" )

if( NOT EXISTS "${GLAD_SRC}" )
    message( FATAL_ERROR "GLAD files not found. Generate them from https://glad.dav1d.de/ with OpenGL 4.6 Core profile and place in glad/ directory" )
endif()

# GLM (header-only math library)
find_path( GLM_INCLUDE_DIR glm/glm.hpp
    PATHS /usr/include /usr/local/include ${CMAKE_SOURCE_DIR}/external
    PATH_SUFFIXES glm
)

if( NOT GLM_INCLUDE_DIR )
    message( WARNING "GLM not found. Installing via FetchContent..." )
    include(FetchContent)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
    )
    FetchContent_MakeAvailable(glm)
    set( GLM_INCLUDE_DIR ${glm_SOURCE_DIR} )
endif()

# --- SOURCE FILES ---
# Automatically find all .cpp files in the src/ directory and subdirectories
file(GLOB_RECURSE ARC-STUDIO-SRC "src/*.cpp")

# Add GLAD source to the list
list(APPEND ARC-STUDIO-SRC ${GLAD_SRC})

# --- SHADERS ---
file( GLOB SHADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert" "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag" )
foreach( shader ${SHADER_FILES} )
    configure_file( ${shader} ${CMAKE_BINARY_DIR}/shaders/ COPYONLY )
endforeach()

# Create executable
add_executable( arc-studio ${ARC-STUDIO-SRC} )

# --- INCLUDE DIRECTORIES ---
target_include_directories(arc-studio PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb
    ${GLM_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries( arc-studio 
    ${OPENGL_LIBRARIES} 
    glfw
    ${CMAKE_DL_LIBS}
)

# Platform-specific settings
if( WIN32 )
    set_target_properties(arc-studio PROPERTIES WIN32_EXECUTABLE TRUE)
    target_link_libraries(arc-studio ${OPENGL_LIBRARIES} glfw)
elseif( APPLE )
    set_target_properties(arc-studio PROPERTIES MACOSX_BUNDLE TRUE)
    target_link_libraries(arc-studio "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()

set_target_properties( arc-studio PROPERTIES LINKER_LANGUAGE CXX )

# Copy assets to build directory
file( COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}" )

install(TARGETS arc-studio
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)