#include "objloader.h"


bool loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
){
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		return false;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}else{
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	for( unsigned int i=0; i<vertexIndices.size(); i++ ){

		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];
		
		out_vertices.push_back(vertex);
		out_uvs     .push_back(uv);
		out_normals .push_back(normal);
	
	}

	return true;
}


void Object::initialize(glm::vec3 position, glm::vec3 scale,TextureLoader textureLoader) {
	_Scale = scale;
	_Position = position;

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	programID = LoadShadersFromFile("../code/Object_Loading/ObjectLoading.vert", "../code/Object_Loading/ObjectLoading.frag");

	mvpMatrixID = glGetUniformLocation(programID, "MVP");
	ModelMatrixID = glGetUniformLocation(programID, "ModelMatrix");

	Texture3DSizeID= glGetUniformLocation(programID,"texture3DSize");
	projectionID= glGetUniformLocation(programID,"projection");

	LayerID= glGetUniformLocation(programID,"Layer");
	SideID= glGetUniformLocation(programID,"Side");

	textureID = textureLoader.LoadTextureTileBox("../code/Textures/Crystals.png");

	textureSamplerID  = glGetUniformLocation(programID, "myTextureSampler");
	texture3DID  = glGetUniformLocation(programID, "voxelTexture");
	cameraposId  = glGetUniformLocation(programID, "viewPos");




	bool res = loadOBJ("../code/Objects/project.obj", vertices, uvs, normals);
	if(!res) {
		std::cout<<"Error during the object import"<<std::endl;
	}

	glGenVertexArrays(1, &Vao);
	glBindVertexArray(Vao);

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glBindVertexArray(0);

	}


void Object::render(glm::mat4 cameraMatrix, int voxel_scene_size, int k,glm::mat4 orthoProjection,int side,GLuint texture3D, glm::vec3 cameraPos) {
	glBindVertexArray(Vao);

	glUseProgram(programID);
	glm::mat4 modelMatrix = glm::mat4();
	modelMatrix = glm::scale(modelMatrix, _Scale);

	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &cameraMatrix[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &orthoProjection[0][0]);
	glUniform3f(cameraposId, cameraPos[0], cameraPos[1], cameraPos[2]);


	glUniform1i(Texture3DSizeID,voxel_scene_size);
	glUniform1i(LayerID,k);
	glUniform1i(SideID,side);

	GLuint ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLight[1]), NULL, GL_STATIC_DRAW);
	PointLight lightData[1] = { glm::vec3(200.0f, 200.0f, 200.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLight), &lightData[0]);
	GLuint positionID = glGetUniformBlockIndex(programID, "pointLights");
	glUniformBlockBinding(programID, positionID, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

	GLuint ubo21;
	glGenBuffers(1, &ubo21);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo21);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), NULL, GL_STATIC_DRAW);
	Material material = {
		glm::vec3(1.f,0.f,0.5f),
		0.5f,
		glm::vec3(1.f,0.f,0.5f),
		0.5f, // "Reflective and refractive" specular diffusion.
		0.5f,
		0, // Emissive materials uses diffuse color as emissive color.
		1.f,
		0.f
	};;
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Material), &material);
	GLuint materialID = glGetUniformBlockIndex(programID, "material");
	glUniformBlockBinding(programID, materialID, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo21);

	GLuint ubo22;
	glGenBuffers(1, &ubo22);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo22);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Settings), NULL, GL_STATIC_DRAW);
	Settings settings = {true,true, true,true};
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Settings), &settings);
	GLuint settingsID = glGetUniformBlockIndex(programID, "settings");
	glUniformBlockBinding(programID, settingsID, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo22);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(textureSamplerID, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, texture3D);
	glUniform1i(texture3DID, 1);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);


	glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);

}


glm::vec3 Object::get_position() {
	return _Position;
}

void Object::set_scale(glm::vec3 scale) {
	_Scale = scale;
}

void Object::cleanup() {
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteBuffers(1, &colorBufferID);
	glDeleteBuffers(1, &indexBufferID);
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteBuffers(1, &uvBufferID);
	glDeleteTextures(1, &textureID);
	glDeleteProgram(programID);
}

std::vector<glm::vec3> Object::get_AABB() {
	glm::vec3 minPoint(FLT_MAX); // Initialize with large values
	glm::vec3 maxPoint(-FLT_MAX); // Initialize with small values
	for(glm::vec3 &vec : vertices) {
		minPoint = glm::min(minPoint, vec);
		maxPoint = glm::max(maxPoint, vec);
	}
	std::vector<glm::vec3> res;
	res.push_back(minPoint);
	res.push_back(maxPoint);
	return res;
}