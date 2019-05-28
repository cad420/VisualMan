# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.3)

project ("MRE")

# Handling function-like macros min() and max() defined in windows.h in MSVC
add_definitions(-DNOMINMAX)

#---------------------------------------------------
# Build Type
#---------------------------------------------------
set(CMAKE_CXX_STANDARD 17)

if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()
#---------------------------------------------------
# Dependency Module
#---------------------------------------------------
# Compile glfw
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
set(GLFW_DIR "lib/glfw")
add_subdirectory(${GLFW_DIR})
add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

#set(_GLFW_INCLUDE_DIR "${GLFW_DIR}/include")								 #glfw headers
#set(_GL3W_INCLUDE_DIR "lib/gl3w")											 #gl3w headers
#include_directories(${_GLFW_INCLUDE_DIR} ${_GL3W_INCLUDE_DIR})			 

# OpenMP
FIND_PACKAGE(OpenMP)
if(OPENMP_FOUND)
message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
message(STATUS "asdfsdf" ${OpenMP_EXE_LINKER_FLAGS})
endif()

add_subdirectory("src")
