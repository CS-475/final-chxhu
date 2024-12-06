/*
 *  Copyright 2024 Christine Hu
 */

#ifndef GSHADER_TRISTICKING_H
#define GSHADER_TRISTICKING_H

#include "include/GShader.h"
#include "include/GMatrix.h"

class GShader_TriSticking : public GShader {
public:
  GShader_TriSticking(GPoint p0, GPoint p1, GPoint p2, GPoint t0, GPoint t1, GPoint t2, GShader* bmShader) : bmShader(bmShader) {
    textureMatrix = getTextureMatrix(p0, p1, p2, t0, t1, t2);
  }

  virtual bool isOpaque() {
    return bmShader->isOpaque();
  }

  // Returns false if the inverse matrix does not exist
  virtual bool setContext(const GMatrix& ctm) {
    return bmShader->setContext(ctm * textureMatrix);
  }

  virtual void shadeRow(int x, int y, int count, GPixel row[]) {
    bmShader->shadeRow(x, y, count, row);
  }

private:
  GShader* bmShader;
  GMatrix textureMatrix;

  GMatrix getTextureMatrix(GPoint p0, GPoint p1, GPoint p2, GPoint t0, GPoint t1, GPoint t2) {
    // 0, 1, 2 -> i, j, k
    GPoint i_k = p0 - p2;
    GPoint j_k = p1 - p2;
    GMatrix P = GMatrix(i_k.x, j_k.x, p2.x, i_k.y, j_k.y, p2.y);
    i_k = t0 - t2;
    j_k = t1 - t2;
    GMatrix T = GMatrix(i_k.x, j_k.x, t2.x, i_k.y, j_k.y, t2.y);
    GMatrix invT = *T.invert();

    return P * invT;
  }

};



#endif //GSHADER_TRISTICKING_H
