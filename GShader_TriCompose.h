/*
 *  Copyright 2024 Christine Hu
 */

#ifndef GSHADER_TRICOMPOSE_H
#define GSHADER_TRICOMPOSE_H

#include "include/GShader.h"


class GShader_TriCompose : public GShader {
public:
  GShader_TriCompose(GShader* gradientShader, GShader* stickingShader) {
    gradShader = gradientShader;
    stickShader = stickingShader;
  }

  virtual bool isOpaque() {
    return gradShader->isOpaque() && stickShader->isOpaque();
  }

  // Returns false if the inverse matrix does not exist
  virtual bool setContext(const GMatrix& ctm) {
    return gradShader->setContext(ctm) && stickShader->setContext(ctm);
  }

  virtual void shadeRow(int x, int y, int count, GPixel row[]) {
    if (count <= 0) {
      return;
    }

    GPixel gradRow[count], stickRow[count];
    gradShader->shadeRow(x, y, count, gradRow);
    stickShader->shadeRow(x, y, count, stickRow);
    if (isOpaque()) {
      for (int i = 0; i < count; i++) {
        GPixel gradPixel = gradRow[i];
        GPixel stickPixel = stickRow[i];
        row[i] = GPixel_PackARGB(
            255,
            (int) std::round(((float) GPixel_GetR(gradPixel) * GPixel_GetR(stickPixel)) / 255.f),
            (int) std::round(((float) GPixel_GetG(gradPixel) * GPixel_GetG(stickPixel)) / 255.f),
            (int) std::round(((float) GPixel_GetB(gradPixel) * GPixel_GetB(stickPixel)) / 255.f)
        );
      }
    } else {
      for (int i = 0; i < count; i++) {
        GPixel gradPixel = gradRow[i];
        GPixel stickPixel = stickRow[i];
        row[i] = GPixel_PackARGB(
            (int) std::round(((float) GPixel_GetA(gradPixel) * GPixel_GetA(stickPixel)) / 255.f),
            (int) std::round(((float) GPixel_GetR(gradPixel) * GPixel_GetR(stickPixel)) / 255.f),
            (int) std::round(((float) GPixel_GetG(gradPixel) * GPixel_GetG(stickPixel)) / 255.f),
            (int) std::round(((float) GPixel_GetB(gradPixel) * GPixel_GetB(stickPixel)) / 255.f)
        );
      }
    }
  }

private:
  GShader* gradShader;
  GShader* stickShader;
};



#endif //GSHADER_TRICOMPOSE_H
