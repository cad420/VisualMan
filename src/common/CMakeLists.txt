
aux_source_directory(. COMMON_SRC)

file(GLOB COMMON_INC "*.h")

add_library(common ${VM_SHARED_OR_STATIC})
target_sources(common PRIVATE ${COMMON_SRC})

install(TARGETS common LIBRARY DESTINATION "lib" RUNTIME DESTINATION "bin" ARCHIVE DESTINATION "lib")
install(FILES ${COMMON_INC} DESTINATION "include")

#set_target_properties(common PROPERTIES DEFINE_SYMBOL)

