
project(ioplugin)

if(WIN32)
	set(SRC_LIST "windowsfilemap.cpp")
	set(INC_LIST "windowsfilemap.h")
elseif(UNIX)
	set(SRC_LIST "linuxfilemap.cpp")
	set(INC_LIST "linuxfilemap.h")
endif()

add_library(ioplugin SHARED)
target_sources(ioplugin PRIVATE ${SRC_LIST})
target_link_libraries(ioplugin common)
target_include_directories(ioplugin PRIVATE "${CMAKE_SOURCE_DIR}/src/common")

install(TARGETS ioplugin LIBRARY DESTINATION "bin" RUNTIME DESTINATION "bin" ARCHIVE DESTINATION "lib")
install(FILES ${INC_LIST} DESTINATION "include")
