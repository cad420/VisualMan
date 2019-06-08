
project(vmglfwwindow)
if(VM_BUILD_GLFW_GUIBINDING)

aux_source_directory(. SRC)

find_package(GLFW3 REQUIRED)


add_library(vmglfwwindow ${VM_SHARED_OR_STATIC})
target_sources(vmglfwwindow PRIVATE ${SRC})
target_link_libraries(vmglfwwindow glfw vm)
target_include_directories(vmglfwwindow PUBLIC "../../src/visualman")

#message(STATUS "-----------------=========================")
# install command

endif()