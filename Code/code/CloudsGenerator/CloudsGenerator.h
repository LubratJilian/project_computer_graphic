#ifndef CLOUDSGENERATOR_H
#define CLOUDSGENERATOR_H

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include<Lights.h>
#include "../render/shader.h"

class PerlinNoiseGenerator{
public:
    PerlinNoiseGenerator(glm::vec3 position,glm::vec3 size);
    void generateTexture(glm::vec3 position, float scale, glm::vec3 numbers, GLuint texture);
    glm::vec3 get_size();
    glm::vec3 get_position();

private:
    float fade(float t);
    float lerp(float t, float a, float b);
    float grad(int hash, float x, float y, float z);
    float perlinNoise3D(float x, float y, float z);
    void generatePermutation();

    glm::vec3 Position;
    std::vector<int> perm;
    glm::vec3 Size;
};

class Grid {
public:
    Grid(glm::vec3 size,glm::vec3 numbers);
    glm::mat4 get_modelMatrix(glm::vec3 camera_position, glm::vec3 grid_position);
    std::vector<GLuint> get_Ids();
    int getIndeiceSize();
    void convert();

private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> uv;
    std::vector<glm::vec3> indices;
    glm::vec3 size;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint uvBufferID;
    GLuint indexBufferID;
    GLuint EBO;

};

class CloudsGenerator {
public :
    CloudsGenerator(glm::vec3 position,glm::vec3 size, glm::vec3 numbers);
    void renderClouds(glm::mat4 projectionMatrix, glm::vec3 cameraPosition, Lights lights);

private :
    PerlinNoiseGenerator generator;
    GLuint programID;
    GLuint mvpMatrixID;
    GLuint textureSampler3DID;
    GLuint textureSampler3DCloudsID;
    GLuint CameraPositionID;
    GLuint nbLightID;
    GLuint blockIndex;
    GLuint vao;
    GLuint ModelID;
    Grid grid;
    GLuint texture;
    glm::vec3 numbersElements;

};

#endif //CLOUDSGENERATOR_H
