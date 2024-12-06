/*
 *  Copyright 2024 Christine Hu
 */

#ifndef GSHADER_GRADIENT2_H
#define GSHADER_GRADIENT2_H

#include "include/GShader.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"

class GShader_Gradient2 : public GShader {
public:
  GShader_Gradient2(GPoint p0, GPoint p1, const GColor colors[], int count, GTileMode tMode) : shaderMatrix(getShaderMatrix(p0, p1)),
                                                                                                  color0(colors[0]),
                                                                                                  color1(colors[1]) {
    contextMatrix = GMatrix();
    invMatrix = shaderMatrix;
    colorDiff = color1 - color0;
    opaque = (color0.a >= 0.9981f) && (color1.a >= 0.9981f);
    tileMode = (int) tMode;
  }

  virtual bool isOpaque() override;

  // Returns false if the inverse matrix does not exist
  virtual bool setContext(const GMatrix& ctm) override;

  virtual void shadeRow(int x, int y, int count, GPixel row[]) override;

private:
  const GMatrix shaderMatrix;
  GMatrix contextMatrix;
  GMatrix invMatrix;
  const GColor color0;
  const GColor color1;
  GColor colorDiff;
  bool opaque;
  int tileMode;

  GMatrix getShaderMatrix(GPoint p0, GPoint p1) {
    float dx = p1.x - p0.x;
    float dy = p1.y - p0.y;
    GMatrix matrix = GMatrix(dx, -dy, p0.x, dy, dx, p0.y);
    return *matrix.invert();
  }

  void shadeRow_kClamp(float x_f, float y_f, int count, GPixel row[]);
  void shadeRow_kRepeat(float x_f, float y_f, int count, GPixel row[]);
  void shadeRow_kMirror(float x_f, float y_f, int count, GPixel row[]);
};



#endif //GSHADER_GRADIENT2_H
