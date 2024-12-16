#version 330 core

in vec3 texCoords; // 3D texture coordinates

out vec4 color;

uniform sampler2DArray clouds;



struct  struct_light {
    mat4 mvpLight;
    vec3 position;
    vec3 color;
    int type;
    float intensity;
    float padding[1]; // 8 bytes (manual padding for custom offset)
};

layout(std140) uniform lights {
    struct_light lights_tab[10]; // Taille maximale du tableau
};


void main() {
    float alpha = smoothstep(0.2, 0.8, texture(clouds,texCoords).r);
    color = vec4(1,1,1,texture(clouds,texCoords).r);
}