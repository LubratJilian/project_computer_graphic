#include<NetworkLoader.h>




bool NetworkLoader::loadPositionFile(const char * path, bool oriented, std::vector<float>& orientations, std::vector<glm::vec3>& positions){
    printf("Loading Position file %s...\n", path);
    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
        return false;
    }
    char lineHeader[128];
    while(1) {
        glm::vec3 vertex;
        float orientation;
		int res = fscanf(file, "%s", &lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
        if(oriented){
            fscanf(file, "%f %f %f %f\n", &vertex.x, &vertex.y, &vertex.z, &orientation);
            orientations.push_back(orientation);
        }
        else {
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
        }
        positions.push_back(vertex);
    }
    return true;
};