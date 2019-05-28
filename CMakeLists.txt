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

# OpenMP
FIND_PACKAGE(OpenMP)
if(OPENMP_FOUND)
message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
message(STATUS ${OpenMP_EXE_LINKER_FLAGS})
endif()

add_subdirectory("src")

add_subdirectory("guibinding")

add_subdirectory("test")
