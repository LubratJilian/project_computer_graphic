#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<iostream>
#include "../render/shader.h"


#ifndef SKYBOX_H
#define SKYBOX_H

class SkyBox{
  public:
	void initialize(glm::vec3 position, glm::vec3 scale, GLuint (*LoadTextureTileBox)(const char *texture_file_path));
	void move(glm::vec3 position);
    void render(glm::mat4 cameraMatrix);
    void cleanup();
    
  private:
	GLuint (*_TextureLoader)(const char *texture_file_path);
    glm::vec3 _Position;		// Position of the box
	glm::vec3 _Scale;		// Size of the box in each axis
	glm::vec3 _Translation;

	GLfloat vertex_buffer_data[72] = {	// Vertex definition for a canonical box
		// Front face
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,


		// Back face
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		// Left face
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		// Right face
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		// Top face
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,


		// Bottom face
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1. , 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,

	};

	GLfloat color_buffer_data[72] = {
		// Front, red
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		// Back, yellow
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		// Left, green
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		// Right, cyan
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,

		// Top, blue
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		// Bottom, magenta
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
	};

	GLuint index_buffer_data[36] = {		// 12 triangle faces of a box
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23,
	};

    // TODO: Define UV buffer data
    // ---------------------------
	GLfloat uv_buffer_data[48] = {
		// Front*
		0.5f, 2./3.,
		0.5f, 1./3.,
		0.25f, 1./3.,
		0.25f, 2./3.,

		// Back
		1.0f, 2./3.,
		1.0f, 1./3.,
		0.75f, 1./3.,
		0.75f, 2./3.,

		// Right
		0.75f, 2./3.,
		0.75f, 1./3.,
		0.5f, 1./3.,
		0.5f, 2./3.,

		// Left
		0.25f, 2./3.,
		0.25f, 1./3.,
		0.0f, 1./3.,
		0.0f, 2./3.,

		// Top - we do not want texture the top
		0.5f, 1./3.,
		0.5f, 0.0f,
		0.25f, 0.0f,
		0.25f, 1./3.,



		// Bottom - we do not want texture the bottom
		0.5f, 1.0f,
		0.5f, 2./3.,
		0.25f, 2./3.,
		0.25f, 1.0f,
		};
    // ---------------------------

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
	GLuint programID;
};

#endif //SKYBOX_H

