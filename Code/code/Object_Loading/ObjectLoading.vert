#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;

uniform mat4 MVP;
uniform mat4 model;

out vec2 UV;
out vec3 Normal;
out vec4 WorldPosition;

void main(){
	gl_Position =  MVP * model * vec4(vertexPosition_modelspace,1);
	UV =  vertexUV;
	Normal = normalize(model * vec4(normal,0)).xyz;
	WorldPosition =  model * vec4(vertexPosition_modelspace,1);
}

