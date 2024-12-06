//
// Created by jilia on 01/12/2024.
//

#ifndef QUAD_H
#define QUAD_H

#include <../render/shader.h>

class Quad{
  public :
  void renderQuad(int level, int k,GLuint voxelTexture);
  void setupQuad();



    private :
  GLuint 	programIDQUAD;

  GLuint QUADID;
  GLuint QUADID2;

  GLuint quadVAO;
  GLuint quadVBO;
  };

#endif //QUAD_H
