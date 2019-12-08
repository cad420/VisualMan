
cmake_minimum_required(VERSION 3.12)
project(blockextractor)

add_executable(blockextractor)
target_sources(blockextractor PRIVATE "blockextractor.cpp")
target_link_libraries(blockextractor vmcore)
target_include_directories(blockextractor PRIVATE)


install(TARGETS blockextractor LIBRARY DESTINATION "lib" RUNTIME DESTINATION "bin" ARCHIVE DESTINATION "lib")