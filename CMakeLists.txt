# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.3)

project ("MRE")


#---------------------------------------------------
# Build Type
#---------------------------------------------------


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
# Package Find
#---------------------------------------------------
find_package(OPENGL REQUIRED)
if(OPENGL_FOUND)
	message(STATUS "Opengl Found:" ${OPENGL_INCLUDE_DIR} ${OPENGL_LIBRARIES})
	include_directories(${OPENGL_INCLUDE_DIR})			#optional
endif(OPENGL_FOUND)



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

# Set headers neede by target
#file(GLOB PROJECT_HEADERS "gui/*.h")
file(GLOB CORE_HEADERS "src/core/*.h")
file(GLOB CAMERAS_HEADERS "src/cameras/*.h")
file(GLOB MATH_HEADERS "src/mathematics/*.h")
file(GLOB MEMORY_HEADERS "src/memory/*.h")
file(GLOB UTILS_HEADERS "src/utility/*.h")
file(GLOB VOLUME_HEADERS "src/volume/*.h")


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
aux_source_directory("src/volume" VOLUME_SRC_LIST)
aux_source_directory("src/core" CORE_SRC_LIST)
aux_source_directory("src/mathematics" MATH_SRC_LIST)
aux_source_directory("src/shape" SHAPE_SRC_LIST)
aux_source_directory("src/utility" UTILS_SRC_LIST)
aux_source_directory("src/render" RENDER_SRC_LIST)


set(GL3W_LIST "lib/gl3w/GL/gl3w.c")



#---------------------------------------------------
# Test Module
#---------------------------------------------------

aux_source_directory("test" TEST_SRC_LIST)
include_directories("test")
message(STATUS "Test source list: ${TEST_SRC_LIST}")

file(GLOB TEST_HEADERS "test/*.h")

add_executable("test" ${TEST_SRC_LIST} ${CORE_SRC_LIST} ${MEM_SRC_LIST} ${VOLUME_SRC_LIST})

#---------------------------------------------------
# MRE Module
#---------------------------------------------------


add_executable(${PROJECT_NAME} ${SRC_LIST} ${GL3W_LIST} ${GUI_SRC_LIST})
target_link_libraries(${PROJECT_NAME} glfw ${OPENGL_LIBRARIES})
set_target_properties(${PROJECT_NAME} PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})


