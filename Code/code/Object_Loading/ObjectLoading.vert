#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

uniform mat4 MVP;
uniform mat4 ModelMatrix;
uniform mat4 projection;
uniform int texture3DSize;


out vec2 UV;
out vec4 worldPosition;


void main(){

	if(texture3DSize !=0 ){
	gl_Position =  projection  *ModelMatrix* vec4(vertexPosition_modelspace,1);

	}
	else{
	gl_Position =  MVP  *ModelMatrix* vec4(vertexPosition_modelspace, 1);
	}
	worldPosition =  projection * ModelMatrix* vec4(vertexPosition_modelspace,1);
	UV = vertexUV;
}

