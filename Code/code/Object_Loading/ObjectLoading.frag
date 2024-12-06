#version 330 core

/*
uniform sampler2D myTextureSampler;
uniform int texture3DSize;
uniform int Layer;
uniform int Side;
uniform sampler3D voxelTexture;   // Voxelized scene




in vec2 UV;
in vec4 worldPosition;
in vec3 Normal;                   // Fragment's Normal vector
out vec4 fragColor;

const int NUM_CONES = 5;          // Number of cones to trace
const int mipLevels = 5;          // Number of cones to trace
const float lightIntensity = 1.f;     // Intensity of the light source
const vec3 lightPos = vec3(200,200,200);            // Position of the direct light
uniform vec3 viewPos; //= vec3(300,0,300);             // Camera/viewer position


// Function to trace a cone through the voxel texture
vec4 coneTrace(vec4 entry, vec3 direction, float coneAngle) {
	vec3 startPos = entry.xyz;
	vec3 stepDir = normalize(direction);  // Normalized direction
	float t = 0;                 // Start step size, grows with mip level
	vec4 accumulatedColor = vec4(0.0);
	float totalWeight = 0.0;

	for (int i = 0; i < 64; ++i) {       // Limit steps for performance
		vec3 samplePos = startPos + stepDir * t;
		int mipLevel = int(log2(t / texture3DSize));  // Select appropriate mip level
		mipLevel = clamp(mipLevel, 0, mipLevels - 1);


		int sliceIndex = 0;
		vec3 posTexture = vec3(0.);
		if (abs(direction.x) > abs(direction.y) && abs(direction.x) > abs(direction.z)) {
			posTexture = direction.x > 0.0 ? vec3(samplePos.z,samplePos.y,ceil(samplePos.x)) : vec3(samplePos.z,samplePos.y,(ceil(samplePos.x)+1)*128);  // +x or -x face
		} else if (abs(direction.y) > abs(direction.z)) {
			posTexture = direction.x > 0.0 ? vec3(samplePos.x,samplePos.z,(ceil(samplePos.y)+1)*128*2) : vec3(samplePos.x,samplePos.z,(ceil(samplePos.y)+1)*128*3);  // +x or -x face
		} else {
			posTexture = direction.x > 0.0 ? vec3(samplePos.x,samplePos.y,(ceil(samplePos.z)+1)*128*4) : vec3(samplePos.x,samplePos.y,(ceil(samplePos.z)+1)*128*5);  // +x or -x face
		}

		// Sample the voxel texture at the correct slice and mip level
		vec4 voxelColor = textureLod(voxelTexture, posTexture, mipLevel);  // Sample the voxel texture array


		//vec4 voxelColor = textureLod(voxelTexture, samplePos, mipLevel);
		float weight = 1.0 / (1.0 + coneAngle * t);  // Weight based on cone angle
		accumulatedColor += voxelColor * weight;
		totalWeight += weight;

		t += texture3DSize;   // Increase step size as we move farther
	}

	return accumulatedColor / totalWeight;  // Normalize by total weight
}

void main() {
	if(texture3DSize != 0)
	{
		if((Side == 0 || Side == 1) && abs(worldPosition.x - Layer) > 1.0){
			discard;
		}
		if((Side == 2 || Side == 3) && abs(worldPosition.y - Layer) > 1.0){
			discard;
		}
		if((Side == 4 || Side == 5) && abs(worldPosition.z - Layer) > 1.0){
			discard;
		}
		else{
			fragColor = vec4((texture( myTextureSampler, UV ).rgb + vec4(0, 0.462, 0.023, 0.480).rgb) / 2,1.0);
		}
	}
	else {

		// Direct lighting using a basic Phong model
		vec3 lightDir = normalize(lightPos - worldPosition.xyz);
		float diff = max(dot(Normal, lightDir), 0.0);
		vec3 diffuse = diff * lightIntensity * vec3(1.0)*((texture( myTextureSampler, UV ).rgb + vec4(0, 0.462, 0.023, 0.480).rgb/2) / 2); // Simple white light, modulated by intensity

		// Specular reflection
		vec3 viewDir = normalize(viewPos - worldPosition.xyz);
		vec3 reflectDir = reflect(-lightDir, Normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
		vec3 specular = spec * lightIntensity * vec3(0.5); // Specular modulated by intensity
		// Indirect lighting (Cone tracing)
		vec3 coneDirs[NUM_CONES] = vec3[NUM_CONES](
		reflect(viewDir, Normal), // Main reflected direction
		vec3(1, 0, 0), vec3(-1, 0, 0), // Spread for more coverage
		vec3(0, 1, 0), vec3(0, -1, 0)
		);

		vec4 indirectColor = vec4(0.0);
		for (int i = 0; i < NUM_CONES; ++i) {
			indirectColor += coneTrace(worldPosition, coneDirs[i], 0.1);
		}

		indirectColor /= NUM_CONES;// Average over all cones

		// Final color combination
		vec3 finalColor =  diffuse + specular + indirectColor.rgb;
		fragColor = vec4(finalColor, 1.0);
	}
}
*/

#define TSQRT2 2.828427
#define SQRT2 1.414213
#define ISQRT2 0.707106
// --------------------------------------
// Light (voxel) cone tracing settings.
// --------------------------------------
#define MIPMAP_HARDCAP 3.4f /* Too high mipmap levels => glitchiness, too low mipmap levels => sharpness. */
#define VOXEL_SIZE (1/128.0) /* Size of a voxel. 128x128x128 => 1/128 = 0.0078125. */
#define SHADOWS 1 /* Shadow cone tracing. */
#define DIFFUSE_INDIRECT_FACTOR 0.52f /* Just changes intensity of diffuse indirect lighting. */
// --------------------------------------
// Other lighting settings.
// --------------------------------------
#define SPECULAR_MODE 1 /* 0 == Blinn-Phong (halfway vector), 1 == reflection model. */
#define SPECULAR_FACTOR 4.0f /* Specular intensity tweaking factor. */
#define SPECULAR_POWER 65.0f /* Specular power in Blinn-Phong. */
#define DIRECT_LIGHT_INTENSITY 0.96f /* (direct) point light intensity factor. */
#define MAX_LIGHTS 1 /* Maximum number of lights supported. */

// Lighting attenuation factors. See the function "attenuate" (below) for more information.
#define DIST_FACTOR 1.1f /* Distance is multiplied by this when calculating attenuation. */
#define CONSTANT 1
#define LINEAR 0 /* Looks meh when using gamma correction. */
#define QUADRATIC 1

// Other settings.
#define GAMMA_CORRECTION 1 /* Whether to use gamma correction or not. */

// Basic point light.
struct PointLight {
	vec3 position;
	vec3 color;
};

// Basic material.
struct Material {
	vec3 diffuseColor;
	float diffuseReflectivity;
	vec3 specularColor;
	float specularDiffusion; // "Reflective and refractive" specular diffusion. 
	float specularReflectivity;
	float emissivity; // Emissive materials uses diffuse color as emissive color.
	float refractiveIndex;
	float transparency;
};

struct Settings {
	bool indirectSpecularLight; // Whether indirect specular light should be rendered or not.
	bool indirectDiffuseLight; // Whether indirect diffuse light should be rendered or not.
	bool directLight; // Whether direct light should be rendered or not.
	bool shadows; // Whether shadows should be rendered or not.
};

uniform Material material;
uniform Settings settings;
uniform PointLight pointLights[MAX_LIGHTS];


const int numberOfLights = 1;            // Position of the direct light
uniform int texture3DSize;
uniform int Layer;
uniform int Side;
uniform vec3 viewPos; // World campera position.
uniform int state; // Only used for testing / debugging.
uniform sampler3D voxelTexture; // Voxelization texture.

in vec3 worldPosition;
in vec3 Normal;
in vec2 UV;

out vec4 color;

uniform sampler2D myTextureSampler;




/*const int NUM_CONES = 5;          // Number of cones to trace
const int mipLevels = 5;          // Number of cones to trace
const float lightIntensity = 1.f;     // Intensity of the light source
const vec3 lightPos = vec3(200,200,200);            // Position of the direct light
*/





vec3 normal = normalize(Normal);
float MAX_DISTANCE = distance(vec3(abs(worldPosition)), vec3(-1));

// Returns an attenuation factor given a distance.
float attenuate(float dist){ dist *= DIST_FACTOR; return 1.0f / (CONSTANT + LINEAR * dist + QUADRATIC * dist * dist); }

// Returns a vector that is orthogonal to u.
vec3 orthogonal(vec3 u){
	u = normalize(u);
	vec3 v = vec3(0.99146, 0.11664, 0.05832); // Pick any normalized vector.
	return abs(dot(u, v)) > 0.99999f ? cross(u, vec3(0, 1, 0)) : cross(u, v);
}

// Scales and bias a given vector (i.e. from [-1, 1] to [0, 1]).
vec3 scaleAndBias(const vec3 p) { return 0.5f * p + vec3(0.5f); }

// Returns true if the point p is inside the unity cube. 
bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

// Returns a soft shadow blend by using shadow cone tracing.
// Uses 2 samples per step, so it's pretty expensive.
float traceShadowCone(vec3 from, vec3 direction, float targetDistance){
	from += normal * 0.05f; // Removes artifacts but makes self shadowing for dense meshes meh.

	float acc = 0;

	float dist = 3 * VOXEL_SIZE;
	// I'm using a pretty big margin here since I use an emissive light ball with a pretty big radius in my demo scenes.
	float STOP = targetDistance - 16 * VOXEL_SIZE;

	while(dist < STOP && acc < 1){
		vec3 c = from + dist * direction;
		if(!isInsideCube(c, 0)) break;
		c = scaleAndBias(c);
		float l = pow(dist, 2); // Experimenting with inverse square falloff for shadows.
		float s1 = 0.062 * textureLod(voxelTexture, c, 1 + 0.75 * l).a;
		float s2 = 0.135 * textureLod(voxelTexture, c, 4.5 * l).a;
		float s = s1 + s2;
		acc += (1 - acc) * s;
		dist += 0.9 * VOXEL_SIZE * (1 + 0.05 * l);
	}
	return 1 - pow(smoothstep(0, 1, acc * 1.4), 1.0 / 1.4);
}

// Traces a diffuse voxel cone.
vec3 traceDiffuseVoxelCone(const vec3 from, vec3 direction){
	direction = normalize(direction);

	const float CONE_SPREAD = 0.325;

	vec4 acc = vec4(0.0f);

	// Controls bleeding from close surfaces.
	// Low values look rather bad if using shadow cone tracing.
	// Might be a better choice to use shadow maps and lower this value.
	float dist = 0.1953125;

	// Trace.
	while(dist < SQRT2 && acc.a < 1){
		vec3 c = from + dist * direction;
		c = scaleAndBias(from + dist * direction);
		float l = (1 + CONE_SPREAD * dist / VOXEL_SIZE);
		float level = log2(l);
		float ll = (level + 1) * (level + 1);
		vec4 voxel = textureLod(voxelTexture, c, min(MIPMAP_HARDCAP, level));
		acc += 0.075 * ll * voxel * pow(1 - voxel.a, 2);
		dist += ll * VOXEL_SIZE * 2;
	}
	return pow(acc.rgb * 2.0, vec3(1.5));
}

// Traces a specular voxel cone.
vec3 traceSpecularVoxelCone(vec3 from, vec3 direction){
	direction = normalize(direction);

	const float OFFSET = 8 * VOXEL_SIZE;
	const float STEP = VOXEL_SIZE;

	from += OFFSET * normal;

	vec4 acc = vec4(0.0f);
	float dist = OFFSET;

	// Trace.
	while(dist < MAX_DISTANCE && acc.a < 1){
		vec3 c = from + dist * direction;
		if(!isInsideCube(c, 0)) break;
		c = scaleAndBias(c);

		float level = 0.1 * material.specularDiffusion * log2(1 + dist / VOXEL_SIZE);
		vec4 voxel = textureLod(voxelTexture, c, min(level, MIPMAP_HARDCAP));
		float f = 1 - acc.a;
		acc.rgb += 0.25 * (1 + material.specularDiffusion) * voxel.rgb * voxel.a * f;
		acc.a += 0.25 * voxel.a * f;
		dist += STEP * (1.0f + 0.125f * level);
	}
	return 1.0 * pow(material.specularDiffusion + 1, 0.8) * acc.rgb;
}

// Calculates indirect diffuse light using voxel cone tracing.
// The current implementation uses 9 cones. I think 5 cones should be enough, but it might generate
// more aliasing and bad blur.
vec3 indirectDiffuseLight(){
	 float ANGLE_MIX = 0.5f; // Angle mix (1.0f => orthogonal direction, 0.0f => direction of normal).

	 float w[3] = {1.0, 1.0, 1.0}; // Cone weights.

	// Find a base for the side cones with the normal as one of its base vectors.
	 vec3 ortho = normalize(orthogonal(normal));
	 vec3 ortho2 = normalize(cross(ortho, normal));

	// Find base vectors for the corner cones too.
	 vec3 corner = 0.5f * (ortho + ortho2);
	 vec3 corner2 = 0.5f * (ortho - ortho2);

	// Find start position of trace (start with a bit of offset).
	 vec3 N_OFFSET = normal * (1 + 4 * ISQRT2) * VOXEL_SIZE;
	 vec3 C_ORIGIN = worldPosition + N_OFFSET;

	// Accumulate indirect diffuse light.
	vec3 acc = vec3(0);

	// We offset forward in normal direction, and backward in cone direction.
	// Backward in cone direction improves GI, and forward direction removes
	// artifacts.
	const float CONE_OFFSET = -0.01;

	// Trace front cone
	acc += w[0] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * normal, normal);

	// Trace 4 side cones.
	 vec3 s1 = mix(normal, ortho, ANGLE_MIX);
	 vec3 s2 = mix(normal, -ortho, ANGLE_MIX);
	 vec3 s3 = mix(normal, ortho2, ANGLE_MIX);
	 vec3 s4 = mix(normal, -ortho2, ANGLE_MIX);

	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * ortho, s1);
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * ortho, s2);
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * ortho2, s3);
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * ortho2, s4);

	// Trace 4 corner cones.
	 vec3 c1 = mix(normal, corner, ANGLE_MIX);
	 vec3 c2 = mix(normal, -corner, ANGLE_MIX);
	 vec3 c3 = mix(normal, corner2, ANGLE_MIX);
	 vec3 c4 = mix(normal, -corner2, ANGLE_MIX);

	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * corner, c1);
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * corner, c2);
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * corner2, c3);
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * corner2, c4);

	// Return result.
	return DIFFUSE_INDIRECT_FACTOR * material.diffuseReflectivity * acc * (material.diffuseColor + vec3(0.001f));
}

// Calculates indirect specular light using voxel cone tracing.
vec3 indirectSpecularLight(vec3 viewDirection){
	 vec3 reflection = normalize(reflect(viewDirection, normal));
	return material.specularReflectivity * material.specularColor * traceSpecularVoxelCone(worldPosition, reflection);
}

// Calculates refractive light using voxel cone tracing.
vec3 indirectRefractiveLight(vec3 viewDirection){
	 vec3 refraction = refract(viewDirection, normal, 1.0 / material.refractiveIndex);
	 vec3 cmix = mix(material.specularColor, 0.5 * (material.specularColor + vec3(1)), material.transparency);
	return cmix * traceSpecularVoxelCone(worldPosition, refraction);
}

// Calculates diffuse and specular direct light for a given point light.  
// Uses shadow cone tracing for soft shadows.
vec3 calculateDirectLight(const PointLight light, const vec3 viewDirection){
	vec3 lightDirection = light.position - worldPosition;
	 float distanceToLight = length(lightDirection);
	lightDirection = lightDirection / distanceToLight;
	 float lightAngle = dot(normal, lightDirection);

	// --------------------
	// Diffuse lighting.
	// --------------------
	float diffuseAngle = max(lightAngle, 0.0f); // Lambertian.	

	// --------------------
	// Specular lighting.
	// --------------------
	#if (SPECULAR_MODE == 0) /* Blinn-Phong. */
	const vec3 halfwayVector = normalize(lightDirection + viewDirection);
	float specularAngle = max(dot(normal, halfwayVector), 0.0f);
	#endif

	#if (SPECULAR_MODE == 1) /* Perfect reflection. */
	 vec3 reflection = normalize(reflect(viewDirection, normal));
	float specularAngle = max(0, dot(reflection, lightDirection));
	#endif

	float refractiveAngle = 0;
	if(material.transparency > 0.01){
		vec3 refraction = refract(viewDirection, normal, 1.0 / material.refractiveIndex);
		refractiveAngle = max(0, material.transparency * dot(refraction, lightDirection));
	}

	// --------------------
	// Shadows.
	// --------------------
	float shadowBlend = 1;
	#if (SHADOWS == 1)
	if(diffuseAngle * (1.0f - material.transparency) > 0 && settings.shadows)
	shadowBlend = traceShadowCone(worldPosition, lightDirection, distanceToLight);
	#endif

	// --------------------
	// Add it all together.
	// --------------------
	diffuseAngle = min(shadowBlend, diffuseAngle);
	specularAngle = min(shadowBlend, max(specularAngle, refractiveAngle));
	 float df = 1.0f / (1.0f + 0.25f * material.specularDiffusion); // Diffusion factor.
	 float specular = SPECULAR_FACTOR * pow(specularAngle, df * SPECULAR_POWER);
	 float diffuse = diffuseAngle * (1.0f - material.transparency);

	 vec3 diff = material.diffuseReflectivity * material.diffuseColor * diffuse;
	 vec3 spec = material.specularReflectivity * material.specularColor * specular;
	 vec3 total = light.color * (diff + spec);
	return attenuate(distanceToLight) * total * vec3((texture( myTextureSampler, UV ).rgb + vec4(0, 0.462, 0.023, 0.480).rgb) / 2);
}

// Sums up all direct light from point lights (both diffuse and specular).
vec3 directLight(vec3 viewDirection){
	vec3 direct = vec3(0.0f);
	for(int i = 0; i < 1; ++i) {
		direct += calculateDirectLight(pointLights[i], viewDirection);
	}
	direct *= DIRECT_LIGHT_INTENSITY;
	return direct;
}

void main(){
if(texture3DSize != 0)
{
	if((Side == 0 || Side == 1) && abs(worldPosition.x - Layer) > 1.0){
		discard;
	}
	if((Side == 2 || Side == 3) && abs(worldPosition.y - Layer) > 1.0){
		discard;
	}
	if((Side == 4 || Side == 5) && abs(worldPosition.z - Layer) > 1.0){
		discard;
	}
	else{
		color = vec4((texture( myTextureSampler, UV ).rgb + vec4(0, 0.462, 0.023, 0.480).rgb) / 2,1.0);
	}
}
else {


	color = vec4(0,0,0,1.); //vec4((texture( myTextureSampler, UV ).rgb + vec4(0, 0.462, 0.023, 0.480).rgb) / 2,1.0);
	vec3 viewDirection = normalize(worldPosition - viewPos);

	// Indirect diffuse light.
	if (settings.indirectDiffuseLight && material.diffuseReflectivity * (1.0f - material.transparency) > 0.01f)
	color.rgb += indirectDiffuseLight();

	// Indirect specular light (glossy reflections).
	if (settings.indirectSpecularLight && material.specularReflectivity * (1.0f - material.transparency) > 0.01f)
	color.rgb += indirectSpecularLight(viewDirection);

	// Emissivity.
	color.rgb += material.emissivity * material.diffuseColor;

	// Transparency
	if (material.transparency > 0.01f)
	color.rgb = mix(color.rgb, indirectRefractiveLight(viewDirection), material.transparency);

	// Direct light.
	if (settings.directLight)
	color.rgb += directLight(viewDirection);

	#if (GAMMA_CORRECTION == 1)
	color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
	#endif

}
}

