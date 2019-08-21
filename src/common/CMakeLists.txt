
aux_source_directory(. COMMON_SRC)

file(GLOB COMMON_INC "*.h")

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  # must compile with -fPIC
  add_compile_options(-fPIC)
endif()
add_library(common ${VM_SHARED_OR_STATIC})
target_sources(common PRIVATE ${COMMON_SRC})
target_include_directories(common PRIVATE "${CMAKE_SOURCE_DIR}/lib/3rdparty")

install(TARGETS common LIBRARY DESTINATION "lib" RUNTIME DESTINATION "bin" ARCHIVE DESTINATION "lib")
install(FILES ${COMMON_INC} DESTINATION "include")

#set_target_properties(common PROPERTIES DEFINE_SYMBOL)

