
project(largevolumerenderer)

find_package(GLFW3 REQUIRED)

find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
	message(STATUS "OpenGL Found:" ${OPENGL_INCLUDE_DIR} ${OPENGL_LIBRARIES})
	include_directories(${OPENGL_INCLUDE_DIR})			#optional
endif(OPENGL_FOUND)

#add_definitions(-DMRE_EXPORT_DLL)
set(GL3W_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/gl3w")
set(LVD_GL3W_SRC "${CMAKE_SOURCE_DIR}/lib/gl3w/GL/gl3w.c")

message(STATUS "Project current dir:" ${CMAKE_SOURCE_DIR})

#include_directories()
#include_directories("../../lib/glfw/include")

aux_source_directory(. LVD_SRC_LIST)
aux_source_directory("../../imgui" LVD_GUI_SRC_LIST)
aux_source_directory("../memory" LVD_MEM_SRC_LIST)
aux_source_directory("../io" LVD_IO_SRC_LIST)
aux_source_directory("../io/platform" LVD_IO_PLATFORM_SRC_LIST)
aux_source_directory("../core" LVD_CORE_SRC_LIST)
aux_source_directory("../mathematics" LVD_MATH_SRC_LIST)
aux_source_directory("../shape" LVD_SHAPE_SRC_LIST)
aux_source_directory("../utility" LVD_UTILS_SRC_LIST)
aux_source_directory("../renderer" LVD_RENDER_SRC_LIST)
aux_source_directory("../gui" LVD_MYGUI_SRC_LIST)
aux_source_directory("../cameras" LVD_CAM_SRC_LIST)
aux_source_directory("../opengl" LVD_OPENGL_SRC_LIST)
aux_source_directory("../../lib/3rdparty" LVD_3RDPARTY_SRC_LIST)
aux_source_directory("../opengl/abstrgpuobject" LVD_ABSTRGPUOBJ_SRC_LIST)


#message(STATUS  ${GL3W_LIST} ${GUI_SRC_LIST} ${IO_SRC_LIST} ${MEM_SRC_LIST} )

add_library(largetvolumerenderer STATIC)
target_sources(largetvolumerenderer PRIVATE
${LVD_SRC_LIST}
${LVD_RENDER_SRC_LIST}
${LVD_GL3W_SRC} 
${LVD_GUI_SRC_LIST} 
${LVD_MYGUI_SRC_LIST} 
${LVD_IO_SRC_LIST} 
${LVD_MEM_SRC_LIST} 
${LVD_CAM_SRC_LIST} 
${LVD_MATH_SRC_LIST}
${LVD_OPENGL_SRC_LIST}
${LVD_UTILS_SRC_LIST} 
${LVD_CORE_SRC_LIST}
${LVD_SHAPE_SRC_LIST}
${LVD_APP_SRC_LIST}
${LVD_ABSTRGPUOBJ_SRC_LIST} ${LVD_IO_PLATFORM_SRC_LIST} ${VD_3RDPARTY_SRC_LIST} "../opengl/lodaggregate.cpp"
)

target_link_libraries(largetvolumerenderer glfw ${OPENGL_LIBRARIES})
target_include_directories(largetvolumerenderer PRIVATE ${GL3W_INCLUDE_DIR})