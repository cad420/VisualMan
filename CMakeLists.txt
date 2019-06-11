
cmake_minimum_required (VERSION 3.8)

project ("VisualMan")
set(CMAKE_CXX_STANDARD 17)				# C++17 is requireddd

if(WIN32)
add_definitions(-DNOMINMAX)			# min and max are recoginized as macro on windows
endif()

if(CMAKE_COMPILER_IS_GNUCXX)		# options for gnu compliler
	add_compile_options(-fpermissive)
	add_compile_options(-std=c++17)
endif()




# Shared or static library
option(VM_SHARED_LIBRARY "Set to On to build using dynimic linking" OFF)
if(VM_SHARED_LIBRARY)
	add_definitions(-DVM_SHARED_LIBRARY)
	set(VM_SHARED_OR_STATIC "SHARED")
else()
	set(VM_SHARED_OR_STATIC "STATIC")
endif()


option(VM_BUILD_OLD_LARGE_VOLUME_RENDERER "Set to On to build the old large volume renderer")


# Test build
option(VM_BUILD_QT5_GUIBINDING "Set to On to build qt5 binding")

option(VM_BUILD_GLFW_GUIBINDING "Set to On to build glfw binding")

option(VM_BUILD_QT5_EXAMPLE "Set to On to build qt5 example")
if(VM_BUILD_QT5_EXAMPLE)
	set(VM_BUILD_QT5_GUIBINDING "On")
endif()

option(VM_BUILD_GLFW_EXAMPLE "Set to On to build glfw example")
if(VM_BUILD_GLFW_EXAMPLE)
	set(VM_BUILD_GLFW_GUIBINDING "On")
endif()

# GuiBinding build


# Tools Build
option(VM_BUILD_TOOLS "Set to On to build tools" ON)


#---------------------------------------------------
# Build Type
#---------------------------------------------------


# OpenMP
FIND_PACKAGE(OpenMP)
if(OPENMP_FOUND)
message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
message(STATUS ${OpenMP_EXE_LINKER_FLAGS})
endif()

add_subdirectory("src")

add_subdirectory("guibinding")

add_subdirectory("test")

if(VM_BUILD_TOOLS)
add_subdirectory("tools")
endif()

#message(STATUS "-------" ${CMAKE_RUNTIME_OUTPUT_DIR})
