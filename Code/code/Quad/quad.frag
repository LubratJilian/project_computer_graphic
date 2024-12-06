
#version 330 core

in vec3 texCoords; // 3D coordinates (x, y, layer)
out vec4 fragColor;

uniform sampler2DArray voxelTexture; // 2D texture array
uniform int prevMipLevel;  // The previous mipmap level (layer index)

void main() {



	// Calculate the offset for downsampling
	vec3 offset = vec3(1.0) / textureSize(voxelTexture, prevMipLevel);

	vec4 color = vec4(0.0);

	// Average 8 neighboring texels for downsampling (8 neighbors in a 3x3x3 pattern)
	for (int x = -1; x <= 1; x += 2) {
		for (int y = -1; y <= 1; y += 2) {
				// Compute the new sample position for each neighbor
				vec3 samplePos = texCoords*textureSize(voxelTexture, prevMipLevel) + vec3(x, y, 0) * offset * 0.5;
				// Sample from the voxel texture array at the current mipmap level
				color += texture(voxelTexture, vec3(samplePos.xy, float(prevMipLevel)));

		}
	}

	fragColor = color / 8.0; // Average the samples for downsampling
}