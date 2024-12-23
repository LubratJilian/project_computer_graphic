#include<StreetLamps.h>

StreetLamps::StreetLamps(TextureLoader textureLoader, NetworkLoader networkLoader) {
    networkLoader.loadPositionFile("../code/Objects/street_lamps.objoriented",true,model_matrices,positions);
    streetLamps = Object(glm::vec3(0,0,0),glm::vec3(100,100,100),textureLoader,"streetlamps.obj","StreetLamp.png");
}

void StreetLamps::render(glm::mat4 projectionMatrix, glm::vec3 cameraPosition, Lights lights) {
    for(int i=0;i<model_matrices.size();i++) {
        streetLamps.set_model_matrix(model_matrices[i]);
        streetLamps.render(projectionMatrix, cameraPosition, lights);
    }
}