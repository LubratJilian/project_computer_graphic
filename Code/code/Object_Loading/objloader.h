#ifndef OBJLOADER_H

#define OBJLOADER_H
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<iostream>
#include <TextureLoader.h>
#include<vector>
#include "../render/shader.h"
#include<Lights.h>

bool loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs, 
	std::vector<glm::vec3> & out_normals
);

class Object{
public:
	void initialize(glm::vec3 position, glm::vec3 scale, TextureLoader textureLoader);
	void render(glm::mat4 cameraMatrix, Lights lights);
	glm::vec3 get_position();
	void set_scale(glm::vec3 scale);
	void cleanup();

private:
	// OpenGL buffers
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint indexBufferID;
	GLuint colorBufferID;
	GLuint uvBufferID;
	GLuint textureID;
	GLuint Vao;
	GLuint blockIndex;

	// Shader variable IDs
	GLuint mvpMatrixID;
	GLuint textureSamplerID;
	GLuint programID;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	glm::vec3 _Scale;
	glm::vec3 _Position;
};

#endif