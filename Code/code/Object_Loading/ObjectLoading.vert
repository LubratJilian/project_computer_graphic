#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;


uniform mat4 MVP;
uniform mat4 ModelMatrix;
uniform mat4 projection;
uniform int texture3DSize;


out vec2 UV;
out vec3 worldPosition;
out vec3 Normal;


void main(){


	vec3 normalizedPos = (vertexPosition_modelspace - vec3(-1.78547,-3.02458,-1.83809)) / (vec3(1.78547,1.69265,1.70402) - vec3(-1.78547,-3.02458,-1.83809));

	// Scale to voxel grid dimensions
	vec3 voxelCoords = normalizedPos * 128.;

	// Transform to clip space (-1 to 1) for rendering
	if(texture3DSize !=0 ){
		gl_Position =  projection  *ModelMatrix* vec4(vertexPosition_modelspace,1);

	}
	else{
	gl_Position =  MVP  *ModelMatrix* vec4(vertexPosition_modelspace, 1);
	}
	worldPosition =  voxelCoords;
	UV = vertexUV;
	Normal = normal;
}

