
project(downsample)
# Set srouce list needed by target
#aux_source_directory("${CMAKE_SOURCE_DIR}/common" SRC_LIST)

add_executable(downsample)
target_sources(downsample PRIVATE "downsample.cpp")
target_link_libraries(downsample common)
target_include_directories(downsample PRIVATE "${CMAKE_SOURCE_DIR}/src/common")

#target_link_libraries(downsample)