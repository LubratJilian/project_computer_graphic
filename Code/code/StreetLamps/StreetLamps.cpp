#include<StreetLamps.h>

StreetLamps::StreetLamps(TextureLoader textureLoader, NetworkLoader networkLoader, Material mat, std::string pathObject, std::string pathTexture,std::string position_path) {
    networkLoader.loadPositionFile(position_path.c_str(),true,model_matrices,positions);
    streetLamps = Object(glm::vec3(0,0,0),glm::vec3(100,100,100),textureLoader,pathObject,pathTexture,mat,"StreetLamps");
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::mat4), &model_matrices[0][0], GL_STATIC_DRAW);

}

void StreetLamps::render(glm::mat4 projectionMatrix, glm::vec3 cameraPosition, Lights lights) {

    //for(int i=0;i<model_matrices.size();i++) {
    ///    streetLamps.set_model_matrix(model_matrices[i]);
    //    streetLamps.render(projectionMatrix, cameraPosition, lights);
    //}
    streetLamps.prepare(projectionMatrix, cameraPosition, lights);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
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