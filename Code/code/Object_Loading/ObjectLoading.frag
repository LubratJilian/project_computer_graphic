#version 330 core

in vec2 UV;
in vec4 worldPosition;

uniform sampler2D myTextureSampler;
uniform int texture3DSize;
uniform int Layer;

layout(location = 0) out vec3 color;

void main() {
	// Basic layer-based filtering
	if(texture3DSize != 0)
	{
		if (abs(worldPosition.z/128. - Layer) > 1.0) {
			color = vec3(1.0,0.0,1.0);

			discard;
		}
		else{
			color = (texture( myTextureSampler, UV ).rgb + vec4(0, 0.462, 0.023, 0.480).rgb) / 2;
		}
	}
	else{
		color = (texture( myTextureSampler, UV ).rgb + vec4(0, 0.462, 0.023, 0.480).rgb) / 2;

	}
	// Store normalized normal
}
