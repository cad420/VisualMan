
project(vmglfwwindow)

aux_source_directory(. SRC)

find_package(GLFW3 REQUIRED)


add_library(vmglfwwindow STATIC)
target_sources(vmglfwwindow PRIVATE ${SRC})
target_link_libraries(vmglfwwindow glfw vm)
target_include_directories(vmglfwwindow PUBLIC "../../src/graphic")