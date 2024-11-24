#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<iostream>
#ifndef CAMERA_H
#define CAMERA_H

class Camera{
  public :
    Camera();
    Camera(glm::vec3 position, float azimuth, float polar, float FoV, float near, float far, float cameraMovementSpeed);
    void set_azimuth(float azimuth);
    void set_polar(float polar);
    void set_position(glm::vec3 position);
    void set_up(glm::vec3 up);
    void set_speed(float cameraMovementSpeed);
    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
    void mouse_callback(GLFWwindow* window, double xoffset, double yoffset);
    glm::mat4 get_MVP();
    glm::vec3 get_position();

  private :
    void calculate_vector();
    glm::vec3 _Position;
    glm::vec3 _LookAt;
    glm::vec3 _Up;
    float _Azimuth;
    float _Polar;
    float _FoV;
    float _Near;
    float _Far;
    float _CameraMovementSpeed;
};

#endif //CAMERA_H
