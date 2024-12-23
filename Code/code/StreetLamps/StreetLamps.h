#ifndef STREETLAMPS_H
#define STREETLAMPS_H

#include <Bot.h>
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
#include <objloader.h>
#include <NetworkLoader.h>

class StreetLamps{
  public:
    StreetLamps(TextureLoader textureLoader, NetworkLoader networkLoader, Material mat, std::string pathObject, std::string pathTexture,std::string position_path, glm::vec3 *scale,bool oriented);
    void render(glm::mat4 projectionMatrix, glm::vec3 cameraPosition, Lights lights);

  private:
    std::vector<glm::vec3> positions;
    Object streetLamps;
    std::vector<glm::mat4> model_matrices;
    std::vector<float> orientations;
  bool _Oriented;
  GLuint VBO;
  GLuint VAO;

};

#endif //STREETLAMPS_H
