#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 color;

layout(location = 4) in vec4 modelCol0;  // Colonne 0 de la matrice de modèle
layout(location = 5) in vec4 modelCol1;  // Colonne 1 de la matrice de modèle
layout(location = 6) in vec4 modelCol2;  // Colonne 2 de la matrice de modèle
layout(location = 7) in vec4 modelCol3;  // Colonne 3 de la matrice de modèle

uniform mat4 MVP;

out vec2 UV;
out vec3 Normal;
out vec4 WorldPosition;
out vec3 Color;


void main(){
	mat4 model = mat4(modelCol0, modelCol1, modelCol2, modelCol3);
	gl_Position =  MVP * model * vec4(vertexPosition_modelspace,1);
	UV =  vertexUV;
	Normal = normalize(model * vec4(normal,0)).xyz;
	WorldPosition =  model * vec4(vertexPosition_modelspace,1);
	Color = color;
}

