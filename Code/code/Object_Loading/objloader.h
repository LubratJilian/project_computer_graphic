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

bool loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs, 
	std::vector<glm::vec3> & out_normals
);

class Object{
public:
	void initialize(glm::vec3 position, glm::vec3 scale, TextureLoader textureLoader);
    void render(glm::mat4 cameraMatrix, int voxel_scene_size, int k, 	glm::mat4 orthoProjection, int side,GLuint texture3D, glm::vec3 cameraPos);
	glm::vec3 get_position();
	void set_scale(glm::vec3 scale);
	void cleanup();
	std::vector<glm::vec3> get_AABB();

private:
	// OpenGL buffers
	GLuint vertexArrayID;
	GLuint projectionID;
	GLuint vertexBufferID;
	GLuint indexBufferID;
	GLuint colorBufferID;
	GLuint uvBufferID;
	GLuint textureID;
	GLuint texture3DID;

	GLuint Vao;
	GLuint SideID;
	GLuint normalBufferID;

	// Shader variable IDs
	GLuint mvpMatrixID;
	GLuint textureSamplerID;
	GLuint programID;
	GLuint LayerID;


	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	glm::vec3 _Scale;
	glm::vec3 _Position;
	GLuint Texture3DSizeID;
	GLuint ModelMatrixID;
	GLuint cameraposId;

	GLuint materialID;
	GLuint positionID;
	GLuint settingsID;


};



struct PointLight {
	glm::vec3 position;
	glm::vec3 color;
};

// Basic material.
struct Material {
	glm::vec3 diffuseColor;
	float diffuseReflectivity;
	glm::vec3 specularColor;
	float specularDiffusion; // "Reflective and refractive" specular diffusion.
	float specularReflectivity;
	float emissivity; // Emissive materials uses diffuse color as emissive color.
	float refractiveIndex;
	float transparency;
};

struct Settings {
	bool indirectSpecularLight; // Whether indirect specular light should be rendered or not.
	bool indirectDiffuseLight; // Whether indirect diffuse light should be rendered or not.
	bool directLight; // Whether direct light should be rendered or not.
	bool shadows; // Whether shadows should be rendered or not.
};

#endif