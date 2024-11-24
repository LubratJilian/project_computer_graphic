cmake_minimum_required(VERSION 3.0)
project(code)

find_package(OpenGL REQUIRED)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_subdirectory(external)

include_directories(
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glad-opengl-3.3/include/
	external/
		code/Camera/
		code/SkyBox/
)

add_executable(main
		code/main.cpp
		code/render/shader.cpp
		code/Camera/Camera.cpp
		code/SkyBox/SkyBox.cpp
)


target_link_libraries(main
		${OPENGL_LIBRARY}
		glfw
		glad
)




