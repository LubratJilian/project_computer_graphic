#include "objloader.h"


bool Object::loadOBJ(
	const char * path
){
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	std::vector<glm::vec3> temp_colors;
	std::string typeStr = "";
	int nb_add = 0;

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
		}else if ( strcmp( lineHeader, "vn" ) == 0 ) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if(colors_activated && strcmp(lineHeader, "usemtl") == 0){
			char type[128];
			fscanf(file, "%s\n", type);
			typeStr = std::string(type);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			if(colors_activated && !typeStr.empty()) {
				for(std::map<std::string,glm::vec3>::iterator it = Colors.begin(); it != Colors.end(); ++it) {
					if(typeStr.find(it->first) != std::string::npos) {
						for(int b=0; b<3;b++) {
							colors.push_back(it->second);
						}
					}
				}
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
		}
			else{
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
		
		vertices.push_back(vertex);
		uvs.push_back(uv);
		normals.push_back(normal);
	
	}

	return true;
}

Object::Object():material(0,0,0,0) {
}

Object::Object(glm::vec3 position, glm::vec3 *scale,TextureLoader textureLoader, std::string nameObj, std::map<std::string,glm::vec3> colors,Material mat): material(mat) {
	Colors = colors;
	colors_activated = true;
	initialize(position, scale,textureLoader, nameObj);
}

Object::Object(glm::vec3 position, glm::vec3 *scale,TextureLoader textureLoader, std::string nameObj, std::string textureName,Material mat): material(mat) {
	textureID = textureLoader.LoadTextureTileBox(("../code/Textures/" + textureName).c_str());
	colors_activated = false;
	initialize(position, scale, textureLoader, nameObj);
}

Object::Object(glm::vec3 position, glm::vec3 *scale,TextureLoader textureLoader, std::string nameObj, std::string textureName,Material mat,std::string shaderName): material(mat) {
	textureID = textureLoader.LoadTextureTileBox(("../code/Textures/" + textureName).c_str());
	colors_activated = false;
	initialize(position, scale, textureLoader, nameObj,shaderName);
}

void Object::set_model_matrix(glm::mat4 model) {
	modelMatrix = model;
}

void Object::set_position(glm::vec3 position) {
	_Position = position;
}


void Object::initialize(glm::vec3 position, glm::vec3 *scale,TextureLoader textureLoader, std::string nameObj){
	_Scale = scale;
	modelMatrix = glm::mat4(1.);
	modelMatrix = glm::scale(modelMatrix, *_Scale);
	_Position = position;

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	programID = LoadShadersFromFile("../code/Object_Loading/ObjectLoading.vert", "../code/Object_Loading/ObjectLoading.frag");

	mvpMatrixID = glGetUniformLocation(programID, "MVP");


	textureSamplerID  = glGetUniformLocation(programID, "myTextureSampler");

	textureSampler3DID  = glGetUniformLocation(programID, "shadows");

	ModelID = glGetUniformLocation(programID, "model");
	CameraPositionID = glGetUniformLocation(programID, "cameraPosition");
	nbLightID = glGetUniformLocation(programID, "numberLights");

	color_activatedID = glGetUniformLocation(programID, "colorActivated");

	blockIndexLight = glGetUniformBlockIndex(programID, "lights");
	blockIndexMaterial = glGetUniformBlockIndex(programID, "material");

	if (blockIndexLight == GL_INVALID_INDEX) {
		std::cerr << "Error: Uniform block 'lights' not found!" << std::endl;
		return;
	}


	bool res = loadOBJ(("../code/Objects/"+nameObj).c_str());
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

	if(colors_activated) {
		glGenBuffers(1, &colorBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);
	}

	glGenBuffers(1, &normalsID);
	glBindBuffer(GL_ARRAY_BUFFER, normalsID);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
	}

void Object::initialize(glm::vec3 position, glm::vec3 *scale,TextureLoader textureLoader, std::string nameObj,std::string shaderName){
	_Scale = scale;

	_Position = position;

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	programID = LoadShadersFromFile(("../code/"+shaderName+"/"+shaderName+".vert").c_str(), ("../code/"+shaderName+"/"+shaderName+".frag").c_str());

	mvpMatrixID = glGetUniformLocation(programID, "MVP");


	textureSamplerID  = glGetUniformLocation(programID, "myTextureSampler");

	textureSampler3DID  = glGetUniformLocation(programID, "shadows");

	ModelID = glGetUniformLocation(programID, "model");
	CameraPositionID = glGetUniformLocation(programID, "cameraPosition");
	nbLightID = glGetUniformLocation(programID, "numberLights");

	color_activatedID = glGetUniformLocation(programID, "colorActivated");

	blockIndexLight = glGetUniformBlockIndex(programID, "lights");
	blockIndexMaterial = glGetUniformBlockIndex(programID, "material");

	if (blockIndexLight == GL_INVALID_INDEX) {
		std::cerr << "Error: Uniform block 'lights' not found!" << std::endl;
		return;
	}


	bool res = loadOBJ(("../code/Objects/"+nameObj).c_str());
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

	if(colors_activated) {
		glGenBuffers(1, &colorBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);
	}

	glGenBuffers(1, &normalsID);
	glBindBuffer(GL_ARRAY_BUFFER, normalsID);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
	}


void Object::render(glm::mat4 projectionMatrix, glm::vec3 cameraPosition, Lights lights) {
	glBindVertexArray(Vao);

	glUseProgram(programID);
	modelMatrix = glm::mat4(1.);
	modelMatrix = glm::scale(modelMatrix, *_Scale);


	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &projectionMatrix[0][0]);

	glUniformMatrix4fv(ModelID, 1, GL_FALSE, &modelMatrix[0][0]);

	glUniform3fv(CameraPositionID, 1, &cameraPosition[0]);

	glUniform1i(nbLightID, lights.get_lights().size());

	glUniform1i(color_activatedID, colors_activated);



	if(!colors_activated) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i(textureSamplerID, 0);
	}


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, lights.get_shadows());
	glUniform1i(textureSampler3DID, 1);

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
	glBindBuffer(GL_ARRAY_BUFFER, normalsID);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	if(colors_activated) {
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glVertexAttribPointer(
			3,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, lights.get_UBO());
	glUniformBlockBinding(programID, blockIndexLight, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, lights.get_UBO());


	glBindBuffer(GL_UNIFORM_BUFFER, material.get_UBO());
	glUniformBlockBinding(programID, blockIndexMaterial, 1);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, material.get_UBO());

	glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);
}

void Object::prepare(glm::mat4 projectionMatrix, glm::vec3 cameraPosition, Lights lights) {
	glBindVertexArray(Vao);

	glUseProgram(programID);



	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &projectionMatrix[0][0]);

	glUniformMatrix4fv(ModelID, 1, GL_FALSE, &modelMatrix[0][0]);

	glUniform3fv(CameraPositionID, 1, &cameraPosition[0]);

	glUniform1i(nbLightID, lights.get_lights().size());

	glUniform1i(color_activatedID, colors_activated);



	if(!colors_activated) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i(textureSamplerID, 0);
	}


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, lights.get_shadows());
	glUniform1i(textureSampler3DID, 1);

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
	glBindBuffer(GL_ARRAY_BUFFER, normalsID);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	if(colors_activated) {
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glVertexAttribPointer(
			3,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, lights.get_UBO());
	glUniformBlockBinding(programID, blockIndexLight, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, lights.get_UBO());


	glBindBuffer(GL_UNIFORM_BUFFER, material.get_UBO());
	glUniformBlockBinding(programID, blockIndexMaterial, 1);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, material.get_UBO());


}

int  Object::getSize() {
	return vertices.size();
}

glm::vec3 Object::get_position() {
	return _Position;
}

void Object::set_scale(glm::vec3 *scale) {
	_Scale = scale;
}

glm::vec3 Object::get_scale() {
	return *_Scale;
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
