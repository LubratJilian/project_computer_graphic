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
    StreetLamps(NetworkLoader networkLoader, std::string position_path,bool oriented, Object* obj);
  StreetLamps(NetworkLoader networkLoader, std::string position_path,bool oriented, Bot* obj);

    void render(glm::mat4 projectionMatrix, glm::vec3 cameraPosition, Lights lights);

  private:
    std::vector<glm::vec3> positions;
    Object* streetLamps;
    std::vector<glm::mat4> model_matrices;
    std::vector<float> orientations;
  Bot* bot;
  std::string type;
  bool _Oriented;
  GLuint VBO;
  GLuint VAO;

};

#endif //STREETLAMPS_H
