
project(ioplugin)


add_library(lvdfilereader SHARED)
target_sources(lvdfilereader PRIVATE "lvdfilereader.cpp")
target_link_libraries(lvdfilereader vmcore)
target_include_directories(lvdfilereader PRIVATE)

#endif()

install(TARGETS lvdfilereader LIBRARY DESTINATION "lib" RUNTIME DESTINATION "bin/plugins" ARCHIVE DESTINATION "lib")
