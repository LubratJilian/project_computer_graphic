#ifndef LIGHTS_H
#define LIGHTS_H

#include<vector>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include<iostream>

enum Light_Type {
    SUN = 0,
    PROJECTOR = 1
};

struct struct_light {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 color;
    alignas(4) int type;
    alignas(4) float intensity;   // add alignas 12 if needed on an empty section
};
typedef struct struct_light light;

class Light{
private:
    glm::vec3 position;
    glm::vec3 color;
    Light_Type type;
    float intensity;
    float _FoV;
    float _Near;
    float _Far;
public :
    Light(glm::vec3 position, glm::vec3 color, float intensity, float FoV, float Near, float Far, Light_Type type);
    float getIntensity();
    Light_Type get_type();
    glm::vec3 get_pos();
    glm::mat4 get_lightVP(glm::vec3 lookAt, glm::vec3 up);
    light convert_light();
};

class Lights{
  private:
    std::vector<Light> lights;
    GLuint shadows;
    GLuint FBO;
    GLuint UBO;
    light converted_lights[10];
public:
    Lights(int width, int height, int number_lights);
    Lights(int width, int height, std::vector<Light> lights);
    void add_light(Light light);
    std::vector<Light> get_lights();
    GLuint get_Fbo();
    GLuint get_shadows();
    void convert_lights();
    void put_data_buffer();
    GLuint get_UBO();
};

#endif