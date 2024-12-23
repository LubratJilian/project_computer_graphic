#include<NetworkLoader.h>

const float PI = 3.14;

float NetworkLoader::convert_angle(float x){
    return -2*PI*x+2*PI;
}

bool NetworkLoader::loadPositionFile(const char * path, bool oriented, std::vector<glm::mat4>& modelMatrices, std::vector<glm::vec3>& positions){
    printf("Loading Position file %s...\n", path);
    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
        return false;
    }
    char lineHeader[128];
    glm::mat4 model;
    while(1) {
        model = glm::mat4();
        glm::vec3 vertex;
        float orientation;
		int res = fscanf(file, "%s", &lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
        model = glm::scale(model,glm::vec3(100,100,100));
        if(oriented){
            fscanf(file, "%f %f %f %f\n", &vertex.x, &vertex.y, &vertex.z, &orientation);
            model = glm::translate(model,vertex);
            model = glm::rotate(model,convert_angle(orientation),glm::vec3(0,1,0));
        }
        else {
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            model = glm::translate(model,vertex);
        }
        modelMatrices.push_back(model);
        positions.push_back(vertex);
    }
    return true;
};