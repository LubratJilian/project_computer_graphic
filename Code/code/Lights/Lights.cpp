#include<Lights.h>

Lights::Lights(int width, int height, int number_lights) {
    glGenBuffers(1, &UBO);
    glGenFramebuffers(1, &FBO);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &shadows);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadows);


    for (int i = 0; i < 1; i++) {
        glTexImage3D(GL_TEXTURE_2D_ARRAY,
        i,                // level
        GL_DEPTH_COMPONENT24,         // Internal format (sized format for color)
        width, height, number_lights*2, // width, height, depth
        0,                // border
        GL_DEPTH_COMPONENT,          // format
        GL_FLOAT, // type
        nullptr);
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadows, 0);
    //glDrawBuffer(GL_DEPTH_ATTACHMENT);
    /*glReadBuffer(GL_NONE);*/

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Shadow framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (glIsTexture(shadows) == GL_FALSE) {
        std::cerr << "voxelTexture is not a valid texture!" << std::endl;
    }
}

Lights::Lights(int width, int height, std::vector<Light> lights) : Lights(width,height,lights.size()) {
    this -> lights  = lights;
}

void Lights::add_light(Light light) {
    lights.push_back(light); // don't forget the size of the texture 2d array can be not good !!!
}

std::vector<Light> Lights::get_lights() {
    return lights;
}

GLuint Lights::get_Fbo() {
    return FBO;
}

GLuint Lights::get_shadows() {
    return shadows;
}

Light::Light(glm::vec3 position, glm::vec3 color, float intensity, float FoV, float Near, float Far, glm::vec3 lookAt, glm::vec3 up, Light_Type type) {
    this -> position = position;
    this -> color = color;
    this -> intensity = intensity;
    this -> type = type;
    _FoV = FoV;
    _Near = Near;
    _Far = Far;
    _LookAt = lookAt;
    _Up = up;
}

float Light::getIntensity() {
    return intensity;
}

glm::mat4 Light::get_lightVP(glm::vec3 lookAt, glm::vec3 up) { // lookAt (0,0,0) up(0,1,0);
    return glm::perspective(glm::radians(_FoV), 4.0f / 3.0f, _Near, _Far) * glm::lookAt(position, lookAt, up);
}

Light_Type Light::get_type() {
    return type;
}

glm::vec3 Light::get_pos() {
    return position;
}

void Lights::convert_lights() {
    for(int i=0;i<10;i++){
        if(lights.size()<=i) {
            converted_lights[i] = {glm::mat4(0),glm::vec3(0,0,0),glm::vec3(0,0,0), 0, 0, {0,0}};
        }
        else {
            converted_lights[i] = lights[i].convert_light();
        }
    }
}

light Light::convert_light() {
    light l = {get_lightVP(_LookAt, _Up), position, color, type, intensity, {0,0}};
    return l;
}

void Lights::put_data_buffer() {
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    convert_lights();
    glBufferData(GL_UNIFORM_BUFFER, sizeof(light)*10, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(light)*10, &converted_lights);
}

GLuint Lights::get_UBO() {
    return UBO;
}

glm::vec3 Light::get_Up() {
    return _Up;
}

glm::vec3 Light::get_LookAt() {
    return _LookAt;
}