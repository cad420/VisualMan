
add_library(vmapps)
aux_source_directory(. VMAPPS_SRC)
target_sources(vmapps PRIVATE ${VMAPPS_SRC})
target_link_libraries(vmapps vm graphicfoundation)
target_include_directories(vmapps PRIVATE "../visualman" "../graphicfoundation")
