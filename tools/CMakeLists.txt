
cmake_minimum_required(VERSION 3.12)
project(blockextractor)

add_executable(blockextractor)
target_sources(blockextractor PRIVATE "blockextractor.cpp")
target_link_libraries(blockextractor vmcore)
target_include_directories(blockextractor PRIVATE)

add_executable(block2raw)
target_sources(block2raw PRIVATE "block2raw.cpp")
target_link_libraries(block2raw vmcore)
target_include_directories(block2raw PRIVATE)


install(TARGETS blockextractor LIBRARY DESTINATION "lib" RUNTIME DESTINATION "bin" ARCHIVE DESTINATION "lib")
install(TARGETS block2raw LIBRARY DESTINATION "lib" RUNTIME DESTINATION "bin" ARCHIVE DESTINATION "lib")