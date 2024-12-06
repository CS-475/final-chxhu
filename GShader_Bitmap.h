/*
 *  Copyright 2024 Christine Hu
 */

#ifndef GSHADER_BITMAP_H
#define GSHADER_BITMAP_H

#include "include/GShader.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"


class GShader_Bitmap : public GShader {
public:
  GShader_Bitmap(const GBitmap& bitmap, const GMatrix& localMatrix, GTileMode tMode) : sBitmap(bitmap), shaderMatrix(localMatrix) {
    contextMatrix = GMatrix();
    invMatrix = *localMatrix.invert();
    width = (float) bitmap.width();
    height = (float) bitmap.height();
    tileMode = (int) tMode;
  }

  virtual bool isOpaque() override;

  // Returns false if the inverse matrix does not exist
  virtual bool setContext(const GMatrix& ctm) override;

  virtual void shadeRow(int x, int y, int count, GPixel row[]) override;

private:
  const GBitmap sBitmap;
  const GMatrix shaderMatrix;
  GMatrix contextMatrix;
  GMatrix invMatrix;
  float width;
  float height;
  int tileMode;

  void shadeRow_kClamp(float a, float b, float px, float py, int count, GPixel row[]);
  void shadeRow_kRepeat(float a, float b, float px, float py, int count, GPixel row[]);
  void shadeRow_kMirror(float a, float b, float px, float py, int count, GPixel row[]);
};



#endif //GSHADER_BITMAP_H
