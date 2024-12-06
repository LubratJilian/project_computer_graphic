//
// Created by jilia on 01/12/2024.
//
#include <Quad.h>
void Quad::setupQuad() {
    float quadVertices[] = {
        // positions        // texCoords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f
    };

    programIDQUAD = LoadShadersFromFile("../code/Quad/quad.vert","../code/Quad/quad.frag");
    if(programIDQUAD == 0) {
        exit(1);
    }

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    QUADID2 = glGetUniformLocation(programIDQUAD, "prevMipLevel");

}


void Quad::renderQuad(int level, int k,GLuint voxelTexture) {

    glBindVertexArray(quadVAO);
    glUseProgram(programIDQUAD);
    glActiveTexture(GL_TEXTURE0);  // Choose the texture unit
    glBindTexture(GL_TEXTURE_2D_ARRAY, voxelTexture);
    glUniform1i(QUADID2,level);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
