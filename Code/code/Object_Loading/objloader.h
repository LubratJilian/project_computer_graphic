#pragma once

#ifndef OBJLOADER_H
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<iostream>
#include "../render/shader.h"

#define OBJLOADER_H

bool loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs, 
	std::vector<glm::vec3> & out_normals
);

class Object{
public:
	void initialize(glm::vec3 position, glm::vec3 scale, GLuint (*LoadTextureTileBox)(const char *texture_file_path));
	void render(glm::mat4 cameraMatrix);

private:
	GLuint (*_TextureLoader)(const char *texture_file_path);

	// OpenGL buffers
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint indexBufferID;
	GLuint colorBufferID;
	GLuint uvBufferID;
	GLuint textureID;

	// Shader variable IDs
	GLuint mvpMatrixID;
	GLuint textureSamplerID;
	GLuint programID2;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	glm::vec3 _Scale;
};

#endif