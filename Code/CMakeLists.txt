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
	code/Object_Loading/
		code/Textures/
		code/Animation/Bot/
		code/Lights/
		code/CloudsGenerator/
)

add_executable(main
		code/main.cpp
		code/render/shader.cpp
		code/Camera/Camera.cpp
		code/SkyBox/SkyBox.cpp
		code/Object_Loading/objloader.cpp
		code/Textures/TextureLoader.cpp
		code/Animation/Bot/Bot.cpp
		code/Lights/Lights.cpp
		code/CloudsGenerator/CloudsGenerator.cpp
)


target_link_libraries(main
		${OPENGL_LIBRARY}
		glfw
		glad
)




