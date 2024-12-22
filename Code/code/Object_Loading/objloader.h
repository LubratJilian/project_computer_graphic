#ifndef OBJLOADER_H

#define OBJLOADER_H
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<iostream>
#include <TextureLoader.h>
#include<vector>
#include<map>
#include "../render/shader.h"
#include<Lights.h>
#include<Camera.h>



class Object{
public:
	Object();
	Object(glm::vec3 position, glm::vec3 scale,TextureLoader textureLoader, std::string nameObj, std::map<std::string,glm::vec3> colors);
	Object(glm::vec3 position, glm::vec3 scale, TextureLoader textureLoader, std::string nameObj, std::string textureName);
	void render(glm::mat4 cameraMatrix, glm::vec3 cameraPosition, Lights lights);
	glm::vec3 get_position();
	void set_scale(glm::vec3 scale);
	void cleanup();
	bool loadOBJ(
	const char * path
);
	void initialize(glm::vec3 position, glm::vec3 scale,TextureLoader textureLoader, std::string nameObj);


private:
	// OpenGL buffers
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint indexBufferID;
	GLuint colorBufferID;
	GLuint uvBufferID;
	GLuint normalsID;
	GLuint textureID;
	GLuint Vao;
	GLuint nbLightID;
	GLuint blockIndex;
	GLuint ModelID;
	GLuint CameraPositionID;
	GLuint color_activatedID;


	// Shader variable IDs
	GLuint mvpMatrixID;
	GLuint textureSamplerID;
	GLuint textureSampler3DID;
	GLuint programID;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> colors;
	glm::vec3 _Scale;
	glm::vec3 _Position;
	std::map<std::string,glm::vec3> Colors;
	bool colors_activated;

};

#endif