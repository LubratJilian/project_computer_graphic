#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexUV;

uniform mat4 MVP;
uniform mat4 ModelMatrix;

out vec3 color;
out vec2 uv;
out vec4 worldPosition;

void main() {
    gl_Position =  MVP *ModelMatrix* vec4(vertexPosition, 1);
    color = vertexColor;
    uv = vertexUV;
    worldPosition = ModelMatrix* vec4(vertexPosition, 1);
}
