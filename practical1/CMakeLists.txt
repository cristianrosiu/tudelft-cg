cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(ComputerGraphics C CXX)

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/framework")
	# Create framework library and include CMake scripts (compiler warnings, sanitizers and static analyzers).
	add_subdirectory("framework")
else()
	# During development the framework lives in parent folder.
	add_subdirectory("../../../framework/" "${CMAKE_BINARY_DIR}/framework/")
endif()

add_executable(Practical1
	"src/main.cpp"
	)
target_compile_features(Practical1 PRIVATE cxx_std_20)
# Link to OpenGL, and Microsoft-GSL and/or make their header files available.
target_link_libraries(Practical1 PRIVATE CGFramework)
enable_sanitizers(Practical1)
set_project_warnings(Practical1)

# Copy all files in the resources folder to the build directory after every successful build.
add_custom_command(TARGET Practical1 POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_CURRENT_LIST_DIR}/resources/" $<TARGET_FILE_DIR:Practical1>)
