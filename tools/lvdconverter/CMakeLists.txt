
add_executable(lvdconverter)
target_sources(lvdconverter PRIVATE "lvdconverter.cpp")
target_link_libraries(lvdconverter common)
target_include_directories(lvdconverter PRIVATE "${CMAKE_SOURCE_DIR}/src/common")