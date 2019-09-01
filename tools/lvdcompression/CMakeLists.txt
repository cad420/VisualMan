


if(VM_BUILD_LZ4ENCODER)
find_path(lz4_INCLUDE_DIR
		  NAMES lz4.h
		  PATHS "${LZ4_SDK_DIR}/include")

find_library(lz4_LIBRARY
	         NAMES lz4
			 PATHS "${LZ4_SDK_DIR}/lib")

set(lz4_INCLUDE_DIRS ${lz4_INCLUDE_DIR})
set(lz4_LIBRARIES ${lz4_LIBRARY})

message(STATUS ------- ${lz4_INCLUDE_DIR})
message(STATUS ------- ${lz4_LIBRARIES})


add_executable(lvd2lz4encoder)
target_sources(lvd2lz4encoder PRIVATE "lvd2lz4encoder.cpp")
target_link_libraries(lvd2lz4encoder common ${lz4_LIBRARIES})
target_include_directories(lvd2lz4encoder PRIVATE "${CMAKE_SOURCE_DIR}/src/common" ${lz4_INCLUDE_DIRS})
install(TARGETS lvd2lz4encoder LIBRARY DESTINATION "lib" RUNTIME DESTINATION "bin" ARCHIVE DESTINATION "lib")
#install(FILES ${INSTALL_INCC} DESTINATION "include")
endif()
