#version 330 core

in vec3 texCoords; // 3D texture coordinates

out vec4 color;

uniform sampler2DArray clouds;

struct struct_Material {
    float diffuse;
    float specular;   // add alignas 12 if needed on an empty section
    float ambient;
    float n;   // add alignas 12 if needed on an empty section
};

layout(std140) uniform material {
    struct_Material m; // Taille maximale du tableau
};


void main() {
    color = vec4(1,1,1,texture(clouds,texCoords).r)*vec4(m.ambient,m.ambient,m.ambient,1);
}