//
// Created by jilia on 23/12/2024.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include<vector>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include<iostream>

struct struct_Material {
    alignas(4) float diffuse;
    alignas(4) float specular;   // add alignas 12 if needed on an empty section
    alignas(4) float ambient;
    alignas(4) float n;   // add alignas 12 if needed on an empty section
};
typedef struct struct_Material material;

class Material {
    public:
        Material();
        Material(float diffuse, float specular, float ambient, float n);
        GLuint get_UBO();
        void change_ambient(float n);
    private:
        material get_converted();
        float _Diffuse{};
        float _Ambient{};
        float _Specular{};
        float n{};
        material converted{};
        GLuint FBO{};
        GLuint UBO{};
};



#endif //MATERIAL_H
