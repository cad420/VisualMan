

add_executable(abcflowgen)
target_sources(abcflowgen PRIVATE "abcflowgen.cpp" "gen.cpp")
install(TARGETS abcflowgen LIBRARY DESTINATION "bin" RUNTIME DESTINATION "bin" ARCHIVE DESTINATION "lib")