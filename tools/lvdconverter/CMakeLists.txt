
add_executable(lvdconverter)


aux_source_directory("${CMAKE_SOURCE_DIR}/src/memory" MEM_SRC_LIST)
aux_source_directory("${CMAKE_SOURCE_DIR}/src/io" IO_SRC_LIST)
aux_source_directory("${CMAKE_SOURCE_DIR}/src/io/platform" IO_PLATFORM_SRC_LIST)
aux_source_directory("${CMAKE_SOURCE_DIR}/src/utility" UTILS_SRC_LIST)
target_sources(lvdconverter PRIVATE "lvdconverter.cpp" ${UTILS_SRC_LIST} ${IO_SRC_LIST} ${MEM_SRC_LIST} ${IO_PLATFORM_SRC_LIST})
target_include_directories(lvdconverter PRIVATE "${CMAKE_SOURCE_DIR}/src/memory" "${CMAKE_SOURCE_DIR}/src/io" "${CMAKE_SOURCE_DIR}/src/io/platform" "${CMAKE_SOURCE_DIR}/src/utility")