#version 330 core

in vec2 UV;

uniform sampler2D myTextureSampler;

out vec3 color;

uniform sampler2DArray shadows;

struct  struct_light {
	vec3 position;
	vec3 color;
	int type;
	float intensity;
};

layout(std140) uniform lights {
	struct_light lights_tab[10]; // Taille maximale du tableau
};

void main(){
	color =  lights_tab[0].color * (texture( myTextureSampler, UV ).rgb + vec4(0, 0.462, 0.023, 0.480).rgb) / 2;
}