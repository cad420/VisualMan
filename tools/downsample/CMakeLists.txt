project(downsample)


# Set srouce list needed by target
aux_source_directory("${CMAKE_SOURCE_DIR}/src/memory" MEM_SRC_LIST)
aux_source_directory("${CMAKE_SOURCE_DIR}/src/io" IO_SRC_LIST)
aux_source_directory("${CMAKE_SOURCE_DIR}/src/io/platform" IO_PLATFORM_SRC_LIST)
aux_source_directory("${CMAKE_SOURCE_DIR}/src/utility" UTILS_SRC_LIST)


add_executable(downsample)
target_sources(downsample PRIVATE "downsample.cpp" ${UTILS_SRC_LIST} ${IO_SRC_LIST} ${MEM_SRC_LIST} ${IO_PLATFORM_SRC_LIST})
target_include_directories(downsample PRIVATE "${CMAKE_SOURCE_DIR}/src/memory" "${CMAKE_SOURCE_DIR}/src/io" "${CMAKE_SOURCE_DIR}/src/io/platform" "${CMAKE_SOURCE_DIR}/src/utility")

#target_link_libraries(downsample)