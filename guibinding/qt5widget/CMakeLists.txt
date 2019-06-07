
project(vmqtwindow)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui OpenGL)

aux_source_directory(. QT5GUI_SRC)

add_library(vmqtwindow STATIC)
target_sources(vmqtwindow PRIVATE ${QT5GUI_SRC})
target_link_libraries(vmqtwindow Qt5::Widgets Qt5::Core Qt5::OpenGL vm)
target_include_directories(vmqtwindow PUBLIC "../../src/visualman")
