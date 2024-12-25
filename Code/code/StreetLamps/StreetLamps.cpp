#include<StreetLamps.h>

StreetLamps::StreetLamps(NetworkLoader networkLoader, std::string position_path,bool oriented, Object* obj){
    networkLoader.loadPositionFile(position_path.c_str(),oriented,orientations,positions);
    _Oriented = oriented;
    streetLamps = obj;
    type = "OBJ";
    glGenBuffers(1, &VBO);
}

StreetLamps::StreetLamps(NetworkLoader networkLoader, std::string position_path,bool oriented, Bot* obj){
    networkLoader.loadPositionFile(position_path.c_str(),oriented,orientations,positions);
    _Oriented = oriented;
    bot = obj;
    type = "BOT";
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
        if(type == "OBJ") {
            model = glm::scale(model,(*streetLamps).get_scale());
        }
        else if(type=="BOT") {
            model = glm::scale(model,(*bot).get_scale());

        }
        model = glm::translate(model,positions[i]);
        if(_Oriented) {
            model = glm::rotate(model,convert_angle(orientations[i]),glm::vec3(0,1,0));

        }
        model_matrices.push_back(model);
    }

    if(type == "OBJ") {
        (*streetLamps).prepare(projectionMatrix, cameraPosition, lights);
    }
    else if(type=="BOT") {
        (*bot).render(projectionMatrix, cameraPosition, lights,true);

    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::mat4), &model_matrices[0][0], GL_STATIC_DRAW);

    for (int i = 0; i < 4; i++) { // Une matrice 4x4 = 4 vec4
        glEnableVertexAttribArray(4 + i);
        glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(4 + i, 1); // Diviseur pour instanciation
    }
    glDrawArraysInstanced(GL_TRIANGLES, 0, (*streetLamps).getSize(), positions.size());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
}