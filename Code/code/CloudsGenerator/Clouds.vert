#version 330 core

layout(location = 0) in vec3 position;  // Quad position
layout(location = 1) in vec3 texcoord;  // Quad position


out vec3 texCoords; // 3D texture coordinates

uniform mat4 model;
uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(position,1);
    texCoords = texcoord; // Pass 3D texture coordinates
}