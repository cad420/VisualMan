
aux_source_directory(. COMMON_SRC)
add_library(common)
target_sources(common PRIVATE ${COMMON_SRC})