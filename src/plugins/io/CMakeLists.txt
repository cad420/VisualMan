
project(ioplugin)

set(INC_LIST "")
set(SRC_LIST "")
if(WIN32)
	set(SRC_LIST ${SRC_LIST} "windowsfilemap.cpp")
	set(INC_LIST ${INC_LIST} "windowsfilemap.h")
elseif(UNIX)
	set(SRC_LIST ${SRC_LIST} "linuxfilemap.cpp")
	set(INC_LIST ${INC_LIST} "linuxfilemap.h")
endif()


#set(LVDFILEREADER_SRC_LIST "lvdfilereader.cpp")
set(INC_LIST ${INC_LIST} "lvdfilereader.h")


add_library(ioplugin SHARED)
target_sources(ioplugin PRIVATE ${SRC_LIST})
target_link_libraries(ioplugin common)
target_include_directories(ioplugin PRIVATE "${CMAKE_SOURCE_DIR}/src/common")

add_library(lvdfilereader SHARED)
target_sources(lvdfilereader PRIVATE "lvdfilereader.cpp")
target_link_libraries(lvdfilereader common)
target_include_directories(lvdfilereader PRIVATE "${CMAKE_SOURCE_DIR}/src/common")

#if(VM_BUILD_LZ4ENCODER)
#find_path(lz4_INCLUDE_DIR
#		  NAMES lz4.h
#		  PATHS "${LZ4_SDK_DIR}/include")

#find_library(lz4_LIBRARY
#	         NAMES lz4
#			 PATHS "${LZ4_SDK_DIR}/lib")

#set(lz4_INCLUDE_DIRS ${lz4_INCLUDE_DIR})
#set(lz4_LIBRARIES ${lz4_LIBRARY})

find_package(lz4 CONFIG REQUIRED)
if(lz4_FOUND)
add_library(lz4filereader SHARED)
target_sources(lz4filereader PRIVATE "lz4filereader.cpp")
target_link_libraries(lz4filereader common lz4::lz4)
target_include_directories(lz4filereader PRIVATE "${CMAKE_SOURCE_DIR}/src/common" ${LZ4_INCLUDE_DIRS})
endif()

#endif()

install(TARGETS ioplugin lvdfilereader lz4filereader LIBRARY DESTINATION "lib" RUNTIME DESTINATION "bin/plugins" ARCHIVE DESTINATION "lib")
install(FILES ${INC_LIST} DESTINATION "include")
