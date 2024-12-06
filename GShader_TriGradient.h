/*
 *  Copyright 2024 Christine Hu
 */

#ifndef GSHADER_TRIGRADIENT_H
#define GSHADER_TRIGRADIENT_H

#include "include/GShader.h"
#include "include/GMatrix.h"


class GShader_TriGradient : public GShader {
public:
  GShader_TriGradient(GPoint p0, GPoint p1, GPoint p2, GColor c0, GColor c1, GColor c2) {
    dColor0 = c0 - c2;
    dColor1 = c1 - c2;
    color2 = c2;
    opaque = c0.a > 0.9981f && c1.a > 0.9981f && c2.a > 0.9981f;
    shaderMatrix = getShaderMatrix(p0, p1, p2);
    contextMatrix = GMatrix();
    invMatrix = shaderMatrix;
  }

  virtual bool isOpaque() override;

  // Returns false if the inverse matrix does not exist
  virtual bool setContext(const GMatrix& ctm) override;

  virtual void shadeRow(int x, int y, int count, GPixel row[]) override;

private:
  GColor dColor0;
  GColor dColor1;
  GColor color2;
  GMatrix shaderMatrix;
  GMatrix contextMatrix;
  GMatrix invMatrix;
  bool opaque;

  GMatrix getShaderMatrix(GPoint Pi, GPoint Pj, GPoint Pk) {
    GPoint Pi_k = Pi - Pk;
    GPoint Pj_k = Pj - Pk;
    GMatrix matrix = GMatrix(Pi_k.x, Pj_k.x, Pk.x, Pi_k.y, Pj_k.y, Pk.y);
    return *matrix.invert();
  }
};



#endif //GSHADER_TRIGRADIENT_H
