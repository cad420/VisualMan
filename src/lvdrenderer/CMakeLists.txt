
project(lvdrenderer)

if(VM_BUILD_OLD_LARGE_VOLUME_RENDERER)


# GL3W 
set(GL3W_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/gl3w")											 #gl3w headers
set(GL3W_SRC "${CMAKE_SOURCE_DIR}/lib/gl3w/GL/gl3w.c")

# OpenGL
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
	message(STATUS "OpenGL Found:" ${OPENGL_INCLUDE_DIR} ${OPENGL_LIBRARIES})
endif(OPENGL_FOUND)

# GLFW3
find_package(GLFW3 REQUIRED)


aux_source_directory(. SRC_LIST)
#aux_source_directory(./abstrgpuobject SUB_SRC_LIST)
aux_source_directory(../../imgui IMGUI_SRC_LIST)


add_executable(lvdrenderer)
target_sources(lvdrenderer PRIVATE ${SRC_LIST} ${GL3W_SRC} ${IMGUI_SRC_LIST})
target_link_libraries(lvdrenderer common graphicfoundation glfw ${OPENGL_LIBRARIES})
target_include_directories(lvdrenderer PUBLIC ${OPENGL_INCLUDE_DIR} ${GL3W_INCLUDE_DIR} "${CMAKE_SOURCE_DIR}/src/common" "${CMAKE_SOURCE_DIR}/src/graphicfoundation" "${CMAKE_SOURCE_DIR}/imgui")
endif()