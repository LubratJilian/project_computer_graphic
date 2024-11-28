#version 330 core

in vec2 UV;

uniform sampler2D myTextureSampler;

out vec3 color;

void main(){
	color = (texture( myTextureSampler, UV ).rgb + vec4(0, 0.462, 0.023, 0.480).rgb) / 2;
}