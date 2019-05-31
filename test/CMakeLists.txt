
#project(vmTest)

# Qt Gui Binding Test

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui OpenGL)

add_executable(vmqtwindow_test)
target_sources(vmqtwindow_test PRIVATE "vmqtwindow_test.cpp")
target_link_libraries(vmqtwindow_test vmqtwindow Qt5::Core)
target_include_directories(vmqtwindow_test PRIVATE ${PROJECT_SOURCE_DIR}/guibinding/qt5widget)

# GLFW GUI Binding Test

add_executable(vmglfwwindow_test)
target_sources(vmglfwwindow_test PRIVATE "vmglfwwindow_test.cpp")
target_link_libraries(vmglfwwindow_test vmglfwwindow vm)
target_include_directories(vmglfwwindow_test PRIVATE ${PROJECT_SOURCE_DIR}/guibinding/glfwwindow ${PROJECT_SOURCE_DIR}/guibinding/graphic)

# Large Volume Renderer Test
add_executable(largevolumerenderer_test)
target_sources(largevolumerenderer_test PRIVATE "largevolume_test.cpp")
target_link_libraries(largevolumerenderer_test largetvolumerenderer)
target_include_directories(largevolumerenderer_test PRIVATE ${PROJECT_SOURCE_DIR}/src/application)

# VM Large Volume Renderer Test

add_executable(vm_largevolumerenderer_test)
target_sources(vm_largevolumerenderer_test PRIVATE "vmlargevolumeraycast_test.cpp")
target_link_libraries(vm_largevolumerenderer_test vmglfwwindow vm)
target_include_directories(vm_largevolumerenderer_test PRIVATE ${PROJECT_SOURCE_DIR}/guibinding/glfwwindow ${PROJECT_SOURCE_DIR}/guibinding/graphic)