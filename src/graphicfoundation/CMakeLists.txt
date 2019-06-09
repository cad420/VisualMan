

add_library(graphicfoundation ${VM_SHARED_OR_STATIC})


file(GLOB INSTALL_INC "*.h")

aux_source_directory(. GRAPHIC_FOUNDATION_SRC)
target_sources(graphicfoundation PRIVATE ${GRAPHIC_FOUNDATION_SRC})
target_link_libraries(graphicfoundation common)
target_include_directories(graphicfoundation PUBLIC ${PROJECT_SOURCE_DIR}/src/common)

install(TARGETS graphicfoundation LIBRARY DESTINATION "bin" RUNTIME DESTINATION "bin" ARCHIVE DESTINATION "lib")
install(FILES ${INSTALL_INCC} DESTINATION "include")
