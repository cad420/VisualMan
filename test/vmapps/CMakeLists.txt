

#if(VM_SHARED_LIBRARY)
#	add_definitions(-DUSE_EXPORT_LIBRARY)
#	set(VM_SHARED_OR_STATIC "SHARED")
#else()
#	add_definitions(-DUSE_IMPORT_LIBRARY)
#	set(VM_SHARED_OR_STATIC "STATIC")
#endif()

file(GLOB INC_LIST "*.h")

add_library(vmapps ${VM_SHARED_OR_STATIC})
aux_source_directory(. VMAPPS_SRC)
target_sources(vmapps PRIVATE ${VMAPPS_SRC})
target_link_libraries(vmapps vm vmcore)
target_include_directories(vmapps PRIVATE "${CMAKE_SOURCE_DIR}/src/visualman")

install(TARGETS vmapps LIBRARY DESTINATION "lib" RUNTIME DESTINATION "bin" ARCHIVE DESTINATION "lib")
install(FILES ${INC_LIST} DESTINATION "include")
install(DIRECTORY "${CMAKE_SOURCE_DIR}/resource/glsl" DESTINATION "bin")
install(DIRECTORY "${CMAKE_SOURCE_DIR}/resource/data" DESTINATION "bin")
