
#project(vmTest)

# Qt Gui Binding Test

if(VM_BUILD_QT5_EXAMPLE OR VM_BUILD_GLFW_EXAMPLE)

add_subdirectory("vmapps")

## example based on qt5 gui
if(VM_BUILD_QT5_EXAMPLE)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui OpenGL)

add_executable(vmqtwindow_test)
target_sources(vmqtwindow_test PRIVATE "vmqtwindow_test.cpp")
target_link_libraries(vmqtwindow_test vmqtwindow Qt5::Core vm vmapps)
target_include_directories(vmqtwindow_test PRIVATE ${PROJECT_SOURCE_DIR}/guibinding/qt5widget ${PROJECT_SOURCE_DIR}/test/vmapps)

endif()		## if(VM_BUILD_QT5_EXAMPLE)

## example base on glfw Gui

if(VM_BUILD_GLFW_EXAMPLE)
## normal
add_executable(vmglfwwindow_test)
target_sources(vmglfwwindow_test PRIVATE "vmglfwwindow_test.cpp")
target_link_libraries(vmglfwwindow_test vmglfwwindow vm vmapps)
target_include_directories(vmglfwwindow_test PRIVATE ${PROJECT_SOURCE_DIR}/guibinding/glfwwindow ${PROJECT_SOURCE_DIR}/src/visualman ${PROJECT_SOURCE_DIR}/test/vmapps)

## VM Large Volume Renderer Test
add_executable(vm_largevolumerenderer_test)
target_sources(vm_largevolumerenderer_test PRIVATE "vmlargevolumeraycast_test.cpp")
target_link_libraries(vm_largevolumerenderer_test vmglfwwindow vm vmapps)
target_include_directories(vm_largevolumerenderer_test PRIVATE ${PROJECT_SOURCE_DIR}/guibinding/glfwwindow ${PROJECT_SOURCE_DIR}/src/visualman ${PROJECT_SOURCE_DIR}/test/vmapps)
endif() ## if(VM_BUILD_GLFW_EXAMPLE)

endif()##if(VM_BUILD_QT5_EXAMPLE OR VM_BUILD_GLFW_EXAMPLE)




# Large Volume Renderer Test

if(VM_BUILD_OLD_LARGE_VOLUME_RENDERER)
#add_executable(largevolumerenderer_test)
#target_sources(largevolumerenderer_test PRIVATE "largevolume_test.cpp")
#target_link_libraries(largevolumerenderer_test lvdrenderer common)
#target_include_directories(largevolumerenderer_test PRIVATE ${PROJECT_SOURCE_DIR}/src/lvdrenderer)
endif()

