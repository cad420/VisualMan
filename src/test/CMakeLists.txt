
#project(vmTest)

# Qt Gui Binding Test
add_executable(vmqtwindow_test)
target_sources(vmqtwindow_test PRIVATE "vmqtwindow_test.cpp")
target_link_libraries(vmqtwindow_test vmqtwindow)
target_include_directories(vmqtwindow_test PRIVATE ${PROJECT_SOURCE_DIR}/src/qt5widget)

# GLFW GUI Binding Test
add_executable(vmglfwwindow_test)
target_sources(vmglfwwindow_test PRIVATE "vmglfwwindow_test.cpp")
target_link_libraries(vmglfwwindow_test vmglfwwindow)
target_include_directories(vmglfwwindow_test PRIVATE ${PROJECT_SOURCE_DIR}/src/glfwwindow ${PROJECT_SOURCE_DIR}/src/graphic)

# Large Volume Renderer Test
add_executable(largevolumerenderer_test)
target_sources(largevolumerenderer_test PRIVATE "largevolume_test.cpp")
target_link_libraries(largevolumerenderer_test largetvolumerenderer)
target_include_directories(largevolumerenderer_test PRIVATE ${PROJECT_SOURCE_DIR}/src/application)
