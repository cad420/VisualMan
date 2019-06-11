
add_executable(lvdconverter)
target_sources(lvdconverter PRIVATE "lvdconverter.cpp")
target_link_libraries(lvdconverter common)
target_include_directories(lvdconverter PRIVATE "${CMAKE_SOURCE_DIR}/src/common")
install(TARGETS lvdconverter LIBRARY DESTINATION "lib" RUNTIME DESTINATION "bin" ARCHIVE DESTINATION "lib")
#install(FILES ${INSTALL_INCC} DESTINATION "include")

