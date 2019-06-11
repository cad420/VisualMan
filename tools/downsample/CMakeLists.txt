
project(downsample)

add_executable(downsample)
target_sources(downsample PRIVATE "downsample.cpp")
target_link_libraries(downsample common)
target_include_directories(downsample PRIVATE "${CMAKE_SOURCE_DIR}/src/common")
install(TARGETS downsample LIBRARY DESTINATION "lib" RUNTIME DESTINATION "bin" ARCHIVE DESTINATION "lib")

#target_link_libraries(downsample)