#include<CloudsGenerator.h>

PerlinNoiseGenerator::PerlinNoiseGenerator(glm::vec3 position,glm::vec3 size) {
    Position = position;
    Size = size;
    generatePermutation();
}

// Fade function to ease coordinate values
float PerlinNoiseGenerator::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

// Linear interpolation
float PerlinNoiseGenerator::lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float PerlinNoiseGenerator::grad(int hash, float x, float y, float z) {
    // Use the hash to pick one of the 12 gradient directions
    int h = hash & 15;  // Take the lowest 4 bits of the hash
    float u = h < 8 ? x : y;  // If the hash is less than 8, use x; otherwise, use y
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);  // Choose between y or z (or x again)

    // Return the dot product
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

// Perlin noise function
float PerlinNoiseGenerator::perlinNoise3D(float x, float y, float z) {
    // Similar to 2D Perlin noise but extended for 3D
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;
    int Z = static_cast<int>(std::floor(z)) & 255;

    x -= std::floor(x);
    y -= std::floor(y);
    z -= std::floor(z);

    float u = fade(x);
    float v = fade(y);
    float w = fade(z);

    int aaa = perm[X + perm[Y + perm[Z]]];
    int aba = perm[X + perm[Y + 1 + perm[Z]]];
    int aab = perm[X + perm[Y + perm[Z + 1]]];
    int abb = perm[X + perm[Y + 1 + perm[Z + 1]]];
    int baa = perm[X + 1 + perm[Y + perm[Z]]];
    int bba = perm[X + 1 + perm[Y + 1 + perm[Z]]];
    int bab = perm[X + 1 + perm[Y + perm[Z + 1]]];
    int bbb = perm[X + 1 + perm[Y + 1 + perm[Z + 1]]];

    float x1, x2, y1, y2;
    x1 = lerp(u, grad(aaa, x, y, z), grad(baa, x - 1, y, z));
    x2 = lerp(u, grad(aba, x, y - 1, z), grad(bba, x - 1, y - 1, z));
    y1 = lerp(v, x1, x2);

    x1 = lerp(u, grad(aab, x, y, z - 1), grad(bab, x - 1, y, z - 1));
    x2 = lerp(u, grad(abb, x, y - 1, z - 1), grad(bbb, x - 1, y - 1, z - 1));
    y2 = lerp(v, x1, x2);

    return glm::clamp(lerp(w, y1, y2),0.f,1.f);// + 1.0f) / 2.0f;
}

// Generate permutation table
void PerlinNoiseGenerator::generatePermutation() {
    std::vector<int> perm(512);
    std::vector<int> p(256);
    for (int i = 0; i < 256; ++i) p[i] = i;

    // Shuffle the permutation table
    std::random_shuffle(p.begin(), p.end());

    for (int i = 0; i < 512; ++i) perm[i] = p[i % 256];

    this->perm = perm;
}

void PerlinNoiseGenerator::generateTexture(glm::vec3 position, float scale, glm::vec3 numbers, GLuint texture) {
    float offsetX = (Position.x - position.x/numbers.x);
    float offsetZ = (Position.z - position.z/numbers.z);

    std::vector<float> noiseData(numbers.x * numbers.y * numbers.z);

	for(int y=0;y<numbers.y;y++) {
		for(int z=0;z<numbers.z;z++) {
			for(int x=0;x<numbers.x;x++) {
				float posx = x*(Size.x/numbers.x)-Size.x/2;
				float posy = y*(Size.y/numbers.y)-Size.y/2;
				float posz = z*(Size.z/numbers.z)-Size.z/2;

				float fx = posx / scale;
				float fy = posy / scale;
				float fz = posz / scale;

				float noiseValue = perlinNoise3D(fx-offsetX, fy,fz-offsetZ); // Or use 3D Perlin noise if needed

				noiseData[y * numbers.x * numbers.z + z * numbers.x + x] = noiseValue;///numbers.y;
			}
		}
	}

    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, numbers.x, numbers.z,numbers.y, GL_RED, GL_FLOAT, noiseData.data());
}


glm::vec3 PerlinNoiseGenerator::get_size() {
	return Size;
}

glm::vec3 PerlinNoiseGenerator::get_position() {
	return Position;
}

Grid::Grid(glm::vec3 size,glm::vec3 numbers) {
	this->size = size;
	int i =0;
	for(int y=0;y<numbers.y+1;y++) {
		for(int z=0;z<numbers.z+1;z++) {
			for(int x=0;x<numbers.x+1;x++) {
				float posx = x*(size.x/numbers.x)-size.x/2;
				float posy = y*(size.y/numbers.y)-size.y/2;
				float posz = z*(size.z/numbers.z)-size.z/2;
				vertices.push_back(glm::vec3(posx,posy,posz));
				uv.push_back(glm::vec3(x/numbers.x,z/numbers.z,y));

				if(x!=numbers.x && z!= numbers.z) {
					indices.push_back(glm::vec3(i,i+1,i+numbers.x+2));
					indices.push_back(glm::vec3(i,i+numbers.x+2,i+numbers.x+1));
				}
				if(y != numbers.y && x!=numbers.x) {
					//indices.push_back(glm::vec3(i,i+1,i+(numbers.x+1)*(numbers.z+1)+1));
					//indices.push_back(glm::vec3(i,i+(numbers.x+1)*(numbers.z+1)+1,i+(numbers.x+1)*(numbers.z+1)));
				}
				if(z != numbers.z && y!=numbers.y) {
					//indices.push_back(glm::vec3(i,i+(numbers.x+1)*(numbers.z+1),i+(numbers.x+1)*(numbers.z+1)+numbers.x+1));
					//indices.push_back(glm::vec3(i,i+(numbers.x+1)*(numbers.z+1)+numbers.x+1,i+numbers.x+1));
				}
				i++;
			}
		}
	}

	// Create a vertex array object
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// Create a vertex buffer object to store the vertex data
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	// --------------------------------------------------------
	// Create a vertex buffer object to store the UV data
	glGenBuffers(1, &uvBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
	glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(glm::vec3), &uv[0],GL_STATIC_DRAW);
	// --------------------------------------------------------

	// Create an index buffer object to store the index data that defines triangle faces
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::vec3), &indices[0], GL_STATIC_DRAW);

	convert();
}

glm::mat4 Grid::get_modelMatrix(glm::vec3 camera_position, glm::vec3 grid_position) {
	glm::mat4 modelMatrix = glm::mat4(1.);
	modelMatrix = glm::translate(modelMatrix,glm::vec3(camera_position.x,grid_position.y,camera_position.z));
	return modelMatrix;
}

std::vector<GLuint> Grid::get_Ids() {
	std::vector<GLuint> res;
	res.push_back(vertexArrayID);
	res.push_back(vertexBufferID);
	res.push_back(uvBufferID);
	res.push_back(EBO);
	return res;
}

int Grid::getIndeiceSize() {
	return indices.size();
}

void Grid::convert() {
	std::vector<GLuint> indices;

	// Conversion des glm::vec3 en GLuint
	for (const auto& tri : this->indices) {
		indices.push_back(static_cast<GLuint>(tri.x));  // Conversion explicite
		indices.push_back(static_cast<GLuint>(tri.y));
		indices.push_back(static_cast<GLuint>(tri.z));
	}

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

}

CloudsGenerator::CloudsGenerator(glm::vec3 position,glm::vec3 size, glm::vec3 numbers, Material mat): generator(position, size),
	grid(size, numbers), material(mat) {
	numbersElements = numbers;
	GLenum glerror = glGetError();
	GLuint VertexArrayID;

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


	programID = LoadShadersFromFile("../code/CloudsGenerator/Clouds.vert", "../code/CloudsGenerator/Clouds.frag");

	mvpMatrixID = glGetUniformLocation(programID, "MVP");


	textureSampler3DCloudsID = glGetUniformLocation(programID, "clouds");


	ModelID = glGetUniformLocation(programID, "model");
	CameraPositionID = glGetUniformLocation(programID, "cameraPosition");

	blockIndexMaterial = glGetUniformBlockIndex(programID, "material");
	if (blockIndexMaterial == GL_INVALID_INDEX) {
		std::cerr << "Error: Uniform block 'lights' not found!" << std::endl;
		return;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, numbers.x, numbers.z, numbers.y,0, GL_RED, GL_INT, nullptr);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//generator.generateTexture(glm::vec3(300,300,300),500,numbersElements, texture);
}

void CloudsGenerator::renderClouds(glm::mat4 projectionMatrix, glm::vec3 cameraPosition, Lights lights) {
	glDepthMask(GL_FALSE);
	generator.generateTexture(cameraPosition,1000,numbersElements, texture);

	std::vector<GLuint> ids = grid.get_Ids();

	glUseProgram(programID);

	glBindVertexArray(ids[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
	glUniform1i(textureSampler3DCloudsID, 0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, ids[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, ids[2]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, material.get_UBO());
	glUniformBlockBinding(programID, blockIndexMaterial, 1);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, material.get_UBO());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ids[3]);

	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &projectionMatrix[0][0]);

	glUniformMatrix4fv(ModelID, 1, GL_FALSE, &grid.get_modelMatrix(cameraPosition, generator.get_position())[0][0]);
	glDrawElements(
		GL_TRIANGLES,      // mode
		grid.getIndeiceSize()*3,    			   // number of indices
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	GLenum error = glGetError();
	if(error != 0) {
		std::cout<<"Error"<<error<<std::endl;
	}
	glDepthMask(GL_TRUE);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}
