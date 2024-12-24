#include<StreetLamps.h>

StreetLamps::StreetLamps(TextureLoader textureLoader, NetworkLoader networkLoader, Material mat, std::string pathObject, std::string pathTexture,std::string position_path, glm::vec3*scale,bool oriented){
    networkLoader.loadPositionFile(position_path.c_str(),oriented,orientations,positions);
    _Oriented = oriented;
    streetLamps = Object(glm::vec3(0,0,0),scale,textureLoader,pathObject,pathTexture,mat,"StreetLamps");
    glGenBuffers(1, &VBO);
}

const float PI = 3.14;

float convert_angle(float x){
    return -2*PI*x+2*PI;
}
void StreetLamps::render(glm::mat4 projectionMatrix, glm::vec3 cameraPosition, Lights lights) {
    model_matrices.clear();
    for(int i=0;i<positions.size();i++) {
        glm::mat4 model = glm::mat4(1);
        model = glm::scale(model,streetLamps.get_scale());
        model = glm::translate(model,positions[i]);
        if(_Oriented) {
            model = glm::rotate(model,convert_angle(orientations[i]),glm::vec3(0,1,0));

        }
        model_matrices.push_back(model);
    }


    streetLamps.prepare(projectionMatrix, cameraPosition, lights);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::mat4), &model_matrices[0][0], GL_STATIC_DRAW);

    for (int i = 0; i < 4; i++) { // Une matrice 4x4 = 4 vec4
        glEnableVertexAttribArray(4 + i);
        glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(4 + i, 1); // Diviseur pour instanciation
    }
    glDrawArraysInstanced(GL_TRIANGLES, 0, streetLamps.getSize(), positions.size());
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
}