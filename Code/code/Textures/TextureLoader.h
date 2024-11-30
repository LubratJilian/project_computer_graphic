#ifndef TEXTURELOADER_H

#define TEXTURELOADER_H
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<iostream>
#include <../external/tinygltf-2.9.3/stb_image.h>

class TextureLoader{
public:
    GLuint LoadTextureTileBox(const char *texture_file_path);

};

#endif //TEXTURELOADER_H
