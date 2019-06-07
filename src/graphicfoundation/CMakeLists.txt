
add_library(graphicfoundation)

aux_source_directory(. GRAPHIC_FOUNDATION_SRC)
target_sources(graphicfoundation PRIVATE ${GRAPHIC_FOUNDATION_SRC})
target_link_libraries(graphicfoundation common)
target_include_directories(graphicfoundation PUBLIC ${PROJECT_SOURCE_DIR}/src/common)