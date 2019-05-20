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

set(_GLFW_INCLUDE_DIR "${GLFW_DIR}/include")								 #glfw headers
set(_GL3W_INCLUDE_DIR "lib/gl3w")											 #gl3w headers
include_directories(${_GLFW_INCLUDE_DIR} ${_GL3W_INCLUDE_DIR})			 


#---------------------------------------------------
add_subdirectory("src/graphic")
#---------------------------------------------------

#---------------------------------------------------
# Package Find
#---------------------------------------------------
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
	message(STATUS "OpenGL Found:" ${OPENGL_INCLUDE_DIR} ${OPENGL_LIBRARIES})
	include_directories(${OPENGL_INCLUDE_DIR})			#optional
endif(OPENGL_FOUND)

# OpenMP
FIND_PACKAGE(OpenMP)
if(OPENMP_FOUND)
message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
message(STATUS "asdfsdf" ${OpenMP_EXE_LINKER_FLAGS})
endif()



#----------------------------------------------------
# Internal
#----------------------------------------------------

# Set include directories
include_directories("src/cameras/")
include_directories("src/core/")
include_directories("src/accelerator/")
include_directories("src/mathematics/")
include_directories("src/memory")
include_directories("src/shape")
include_directories("src/utility")
include_directories("src/opengl")
include_directories("lib/3rdparty")


# Set headers neede by target
#file(GLOB PROJECT_HEADERS "gui/*.h")
file(GLOB CORE_HEADERS "src/core/*.h")
file(GLOB CAMERAS_HEADERS "src/cameras/*.h")
file(GLOB MATH_HEADERS "src/mathematics/*.h")
file(GLOB MEMORY_HEADERS "src/memory/*.h")
file(GLOB UTILS_HEADERS "src/utility/*.h")
file(GLOB VOLUME_HEADERS "src/io/*.h")


message(STATUS "PROJECT_HEADERS:${PROJECT_HEADERS}")
message(STATUS "CORE_HEADERS:${CORE_HEADERS}")
message(STATUS "MATH_HEADERS:${MATH_HEADERS}")
#file(GLOB UTILS_HEADERS "src/utils/*.h")
message(STATUS ${PROJECT_HEADERS})
#include_directories(${PROJECT_HEADERS})

# Set srouce list needed by target
aux_source_directory("src" SRC_LIST)
aux_source_directory("imgui" GUI_SRC_LIST)
aux_source_directory("src/memory" MEM_SRC_LIST)
aux_source_directory("src/io" IO_SRC_LIST)
aux_source_directory("src/io/platform" IO_PLATFORM_SRC_LIST)
aux_source_directory("src/core" CORE_SRC_LIST)
aux_source_directory("src/mathematics" MATH_SRC_LIST)
aux_source_directory("src/shape" SHAPE_SRC_LIST)
aux_source_directory("src/utility" UTILS_SRC_LIST)
aux_source_directory("src/renderer" RENDER_SRC_LIST)
aux_source_directory("src/gui" MYGUI_SRC_LIST)
aux_source_directory("src/cameras" CAM_SRC_LIST)
aux_source_directory("src/opengl" OPENGL_SRC_LIST)
aux_source_directory("src/gui" MYGUI_SRC_LIST)
aux_source_directory("lib/3rdparty" 3RDPARTY_SRC_LIST)
aux_source_directory("src/opengl/abstrgpuobject" ABSTRGPUOBJ_SRC_LIST)


set(GL3W_LIST "lib/gl3w/GL/gl3w.c")


#---------------------------------------------------
# Test Module
#---------------------------------------------------

# Gernerator Example
aux_source_directory("test" TEST_SRC_LIST)
include_directories("test")
message(STATUS "Test source list: ${TEST_SRC_LIST}")

file(GLOB TEST_HEADERS "test/*.h")

#add_executable("mytest" 
#"test/test.cpp"
#"test/abcflowgen.cpp"
#${3RDPARTY_SRC_LIST}
#${CORE_SRC_LIST} 
#${MEM_SRC_LIST} 
#${IO_SRC_LIST} 
#${UTILS_SRC_LIST}
#${MATH_SRC_LIST}
#src/algorithm/marchingcubes.cpp src/algorithm/marchingcubes.h)

# Application Test

aux_source_directory("src/application" APP_SRC_LIST)


#---------------------------------------------------
# Binary
#---------------------------------------------------

#---------------------------------------------------
# application_test test
#---------------------------------------------------
add_executable("ApplicationTest" 
"test/application_test.cpp"
${CORE_SRC_LIST}
${UTILS_SRC_LIST}
${MYGUI_SRC_LIST} 
${GUI_SRC_LIST}
${IO_SRC_LIST} 
${MEM_SRC_LIST} 
${MATH_SRC_LIST}
${GL3W_LIST}
${OPENGL_SRC_LIST}
${APP_SRC_LIST}
${CAM_SRC_LIST} 
${RENDER_SRC_LIST} src/algorithm/marchingcubes.cpp src/algorithm/marchingcubes.h)
target_link_libraries(ApplicationTest glfw ${OPENGL_LIBRARIES})

#---------------------------------------------------
# test test
#---------------------------------------------------
add_executable("test" "test/test.cpp" ${UTILS_SRC_LIST})

#---------------------------------------------------
# graphicapi test
#---------------------------------------------------
add_executable("graphicsapi" 
"test/newgraphicapi_test.cpp" 
"src/application/glfwapplication2.cpp"
"src/utility/error.cpp"
"src/graphic/graphicsbooster.cpp")
target_link_libraries("graphicsapi" rasterizer glfw)

#---------------------------------------------------
# printvolume test
#---------------------------------------------------
add_executable("printvolume" "test/printvolume.cpp" ${IO_SRC_LIST} ${MEM_SRC_LIST})

#--------------------------------------------
# Tool: LVDConverter
#--------------------------------------------
add_executable("lvdconverter" 
"tools/lvdconverter/lvdconverter" 
${CORE_SRC_LIST} 
${MEM_SRC_LIST} 
${IO_SRC_LIST} 
${IO_PLATFORM_SRC_LIST}
${UTILS_SRC_LIST}
${MATH_SRC_LIST}
)

#---------------------------------------------
# Tool: Downsample
#---------------------------------------------
add_executable("downsample" 
"tools/downsample/downsample"
${UTILS_SRC_LIST}
${IO_SRC_LIST} 
${MEM_SRC_LIST} 
${IO_PLATFORM_SRC_LIST})
#target_link_libraries(downsample)


#---------------------------------------------------
# Main: MRE project
#---------------------------------------------------
add_executable(
${PROJECT_NAME} 
${RENDER_SRC_LIST}
${SRC_LIST} 
${GL3W_LIST} 
${GUI_SRC_LIST} 
${MYGUI_SRC_LIST} 
${IO_SRC_LIST} 
${MEM_SRC_LIST} 
${CAM_SRC_LIST} 
${MATH_SRC_LIST}
${OPENGL_SRC_LIST}
${UTILS_SRC_LIST} 
${CORE_SRC_LIST}
${SHAPE_SRC_LIST}
${APP_SRC_LIST}
${ABSTRGPUOBJ_SRC_LIST} ${IO_PLATFORM_SRC_LIST} "src/opengl/lodaggregate.cpp")
target_link_libraries(${PROJECT_NAME} glfw ${OPENGL_LIBRARIES})
set_target_properties(${PROJECT_NAME} PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
