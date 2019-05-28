
project(vmglfwwindow)

aux_source_directory(. SRC)
#file(GLOB vmqtwindow_INCLUDE_DIRS "*.h")


add_library(vmglfwwindow STATIC)
target_sources(vmglfwwindow PRIVATE ${SRC})
target_link_libraries(vmglfwwindow glfw vm)