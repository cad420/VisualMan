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

# include dir

include_directories("src/cameras/")
include_directories("src/core/")


# Include sub-projects.
# add_subdirectory ("src")


# Set headers neede by target
file(GLOB PROJECT_HEADERS "gui/*.h")
file(GLOB CORE_HEADERS "src/core/*.h")
file(GLOB CAMERAS_HEADERS "src/core/*.h")
#file(GLOB UTILS_HEADERS "src/utils/*.h")
message(STATUS ${PROJECT_HEADERS})
include_directories(${PROJECT_HEADERS})



# Set srouce list needed by target
aux_source_directory("src" SRC_LIST)
aux_source_directory("src/gui" GUI_SRC_LIST)
set(GL3W_LIST "lib/gl3w/GL/gl3w.c")
# message(STATUS "Src List: ${SRC_LIST} ${GL3W_LIST} ${GUI_SRC_LIST}")

add_executable(${PROJECT_NAME} ${SRC_LIST} ${GL3W_LIST} ${GUI_SRC_LIST})

target_link_libraries(${PROJECT_NAME} glfw ${OPENGL_LIBRARIES})
set_target_properties(${PROJECT_NAME} PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
