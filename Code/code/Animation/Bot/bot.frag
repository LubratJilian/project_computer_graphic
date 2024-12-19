#version 330 core

in vec2 UV;
in vec4 WorldPosition;
in vec3 worldNormal;

out vec4 out_color;

uniform sampler2DArray shadows;

uniform int numberLights;
uniform vec3 cameraPosition;

struct  struct_light {
	mat4 mvpLight;
	vec3 position;
	vec3 color;
	int type;
	float intensity;
	float padding[1]; // 8 bytes (manual padding for custom offset)
};

layout(std140) uniform lights {
	struct_light lights_tab[10]; // Taille maximale du tableau
};

float calcul_shadow(int i){

	vec4 worldPosition = lights_tab[i].mvpLight * WorldPosition;

	vec3 projCoords = worldPosition.xyz / worldPosition.w;
	vec3 UVCoords = projCoords * 0.5 + 0.5;

	float currentDepth = UVCoords.z;
	float shadow;
	if(worldPosition.z>=0){
		float closestDepth = texture(shadows,vec3(UVCoords.xy,i*2)).r;
		shadow = currentDepth >= closestDepth + 1e-3 ? 0.2 : 1.0;
	}
	else{
		float closestDepth = texture(shadows,vec3(UVCoords.xy,i*2+1)).r;
		shadow = currentDepth <= closestDepth + 1e-3 ?0.2 : 1.0;
	}
	return shadow;
}

void main(){
	//TODO add something like materials
	float diffuse = 1;
	float specular = 1;
	float ambient_term = 0.1;
	int n=100;

	vec3 color = vec3(0,0,0);
	for(int i=0;i<numberLights;i++){

		vec3 light_direction = normalize(lights_tab[i].position-WorldPosition.xyz);
		float diffuse_term = diffuse * max(dot(light_direction,worldNormal),0.0);

		vec3 reflected_vector = normalize(light_direction-2*dot(worldNormal,light_direction)*worldNormal);
		vec3 camera_direction = normalize(cameraPosition-WorldPosition.xyz);

		float specular_term = specular*pow(max(dot(reflected_vector,camera_direction),0.0),n);

		float r = pow(length(lights_tab[i].position-WorldPosition.xyz),2);
		float common_term = lights_tab[i].intensity/(4*3.14*r);

		float shadow = (calcul_shadow(i));

		vec3 add_color = vec3(shadow)* lights_tab[i].color*vec3(common_term) * (specular_term + diffuse_term + ambient_term);
		color += add_color;
		//color =  lights_tab[0].intensity * lights_tab[0].color * (texture( myTextureSampler, UV ).rgb + vec4(0, 0.462, 0.023, 0.480).rgb) / 2;
	}

	color = color/(1+color);
	color = pow(color,vec3(1.0/2.2));
	out_color = vec4(color,1);
	//color = vec3(1-calcul_shadow(0));
}