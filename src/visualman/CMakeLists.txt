
cmake_minimum_required (VERSION 3.3)
project (vm)


set(GL3W_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/gl3w")											 #gl3w headers
set(GL3W_SRC "${CMAKE_SOURCE_DIR}/lib/gl3w/GL/gl3w.c")

# GL3W module shoule be moved into this folder later

find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
	message(STATUS "OpenGL Found:" ${OPENGL_INCLUDE_DIR} ${OPENGL_LIBRARIES})
	include_directories(${OPENGL_INCLUDE_DIR})			#optional
endif(OPENGL_FOUND)


set(GRAPHIC_SRC)
aux_source_directory(. GRAPHIC_SRC)

file(GLOB VM_INC "*.h")


set(GRAPHIC_INCLUDE)


#message(STATUS  ${GRAPHIC_SRC})

add_library(vm ${VM_SHARED_OR_STATIC}
${GRAPHIC_SRC} 
${GL3W_SRC})
target_link_libraries(vm ${OPENGL_LIBRARIES} common graphicfoundation)
target_include_directories(vm PUBLIC ${GL3W_INCLUDE_DIR} "../common" "../graphicfoundation")

install(TARGETS vm LIBRARY DESTINATION "lib" RUNTIME DESTINATION "bin" ARCHIVE DESTINATION "lib")
install(FILES ${VM_INC} DESTINATION "include")
install(DIRECTORY ${GL3W_INCLUDE_DIR} DESTINATION "include")
#install(FILES ${VN_INC} DESTINATION "include")