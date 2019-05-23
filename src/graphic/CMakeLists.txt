
cmake_minimum_required (VERSION 3.3)


project ("rasterizer")

add_definitions(-DMRE_EXPORT_DLL)
set(_GL3W_INCLUDE_DIR "lib/gl3w")											 #gl3w headers
include_directories(${_GL3W_INCLUDE_DIR})
set(GL3W_SRC "../../lib/gl3w/GL/gl3w.c")


find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
	message(STATUS "OpenGL Found:" ${OPENGL_INCLUDE_DIR} ${OPENGL_LIBRARIES})
	include_directories(${OPENGL_INCLUDE_DIR})			#optional
endif(OPENGL_FOUND)


set(GRAPHIC_SRC)
aux_source_directory(. GRAPHIC_SRC)
aux_source_directory("../mathematics" MATH_SRC)
aux_source_directory("../memory" MEMORY_SRC)
aux_source_directory("../utility" UTILITY_SRC)


set(GRAPHIC_INCLUDE)

file(GLOB GRAPHIC_INCLUDE "*.h")



#message(STATUS  ${GRAPHIC_SRC})

add_library("rasterizer" STATIC 
${GRAPHIC_SRC} 
${MATH_SRC} 
${GL3W_SRC} 
${MEMORY_SRC} 
${UTILITY_SRC}
"../io/rawreader.cpp"
"../io/rawio.cpp"
"../io/platform/windowsfilemap.cpp")

target_link_libraries("rasterizer" ${OPENGL_LIBRARIES})