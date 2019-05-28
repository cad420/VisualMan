project(downsample)


# Set srouce list needed by target
aux_source_directory("../../src/memory" MEM_SRC_LIST)
aux_source_directory("../../src/io" IO_SRC_LIST)
aux_source_directory("../../src/io/platform" IO_PLATFORM_SRC_LIST)
aux_source_directory("../../src/utility" UTILS_SRC_LIST)

add_executable(
downsample 
${UTILS_SRC_LIST}
${IO_SRC_LIST} 
${MEM_SRC_LIST} 
${IO_PLATFORM_SRC_LIST})

target_sources(downsample PRIVATE  "downsample.cpp")
target_include_directories(downsample ${})

#target_link_libraries(downsample)