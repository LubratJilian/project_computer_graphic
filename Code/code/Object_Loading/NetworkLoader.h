#ifndef NETWORKLOADER_H
#define NETWORKLOADER_H
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

class NetworkLoader {
public:
    bool loadPositionFile(const char * path, bool oriented, std::vector<glm::mat4>& modelMatrices, std::vector<glm::vec3>& positions);
private:
    float convert_angle(float x);
};

#endif //NETWORKLOADER_H
