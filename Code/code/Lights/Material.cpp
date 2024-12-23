#include "Material.h"

Material::Material(float diffuse, float specular, float ambient, float n) {
    _Diffuse = diffuse;
    _Specular = specular;
    _Ambient = ambient;
    this->n = n;
    converted = {_Diffuse, _Specular, _Ambient,this->n};


    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(material), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(material), &converted);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

material Material::get_converted() {
    return converted;
}

GLuint Material::get_UBO() {
    return UBO;
}