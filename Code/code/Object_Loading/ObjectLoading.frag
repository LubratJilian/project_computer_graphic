#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main(){

	// Output color = color of the texture at the specified UV
	color = (texture( myTextureSampler, UV ).rgb + vec4(0, 0.462, 0.023, 0.480).rgb) / 2;
}