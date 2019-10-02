

project(vmqtwindow)

if(VM_BUILD_QT5_GUIBINDING)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui OpenGL)


aux_source_directory(. QT5GUI_SRC)
set(INC_LIST "vmqtwindow.h")

add_library(vmqtwindow SHARED)
target_sources(vmqtwindow PRIVATE ${QT5GUI_SRC})
target_link_libraries(vmqtwindow Qt5::Widgets vm)
target_include_directories(vmqtwindow PUBLIC "../../src/visualman")

install(TARGETS vmqtwindow LIBRARY DESTINATION "lib" RUNTIME DESTINATION "bin" ARCHIVE DESTINATION "lib")
install(FILES ${INC_LIST} DESTINATION "include")

endif()
