/*
 *  Copyright 2024 Christine Hu
 */

#ifndef GSHADER_POSGRADIENT_H
#define GSHADER_POSGRADIENT_H

#include "include/GShader.h"
#include "include/GMatrix.h"
#include <iostream>

class GShader_PosGradient : public GShader {
public:
  GShader_PosGradient(GPoint p0, GPoint p1, const GColor colors[], const float pos[], int count) : num(count) {
    shaderMatrix = getShaderMatrix(p0, p1);
    contextMatrix = GMatrix();
    invMatrix = shaderMatrix;
    color.assign(colors, colors + count);
    position.assign(pos, pos + count);
    colorDiff.resize(count - 1);
    positionDiff.resize(count - 1);
    opaque = colors[0].a <= 0.9981f;
    for (int i=1; i<count; i++) {
      if (colors[i].a <= 0.9981f) {
        opaque = false;
      }
      colorDiff[i-1] = colors[i] - colors[i-1];
      if (position[i] == position[i-1]) {
        positionDiff[i-1] = 0;
      } else {
        positionDiff[i-1] = 1 / (position[i] - position[i-1]);
      }
    }
  }

  virtual bool isOpaque() {
    return opaque;
  }

  // Returns false if the inverse matrix does not exist
  virtual bool setContext(const GMatrix& ctm) {
    if (contextMatrix == ctm) {
      return true;
    }

    nonstd::optional<GMatrix> invCTMPointer = ctm.invert();

    if (invCTMPointer.has_value()) {
      contextMatrix = ctm;
      invMatrix = shaderMatrix * *invCTMPointer;
      return true;
    }
    return false;
  }

  virtual void shadeRow(int x, int y, int count, GPixel row[]) {
    float x_f = x + 0.5f;
    float y_f = y + 0.5f;
    float px = invMatrix[0]*x_f + invMatrix[2]*y_f + invMatrix[4];
    float dx = invMatrix[0];

    if (opaque) {
      for (int i = 0; i < count; i++) {
        // Clamp and Scale
        float ix = fmin(fmax(px, 0.f), 0.9999f);
        // Determine C0's index and distance from px
        int lesserIndex = findPosIndex(ix);
        float lesserPos = position[lesserIndex];
        float t = (ix - lesserPos) * positionDiff[lesserIndex];
        std::cout << "t: " << t << "\n";
        // Color Calculation
        GColor newColor = color[lesserIndex] + colorDiff[lesserIndex]*t;
        // GColor --> GPixel
        int r = (int) std::round(newColor.r * 255);
        int g = (int) std::round(newColor.g * 255);
        int b = (int) std::round(newColor.b * 255);
        row[i] = GPixel_PackARGB(255, r, g, b);

        // Increment x before moving onto the next row pixel
        px += dx;
      }
    } else {
      for (int i = 0; i < count; i++) {
        // Clamp and Scale
        float ix = fmin(fmax(px, 0.f), 0.9999f);
        // Determine C0's index and distance from ix
        int lesserIndex = findPosIndex(ix);
        float lesserPos = position[lesserIndex];
        float t = (ix - lesserPos) * positionDiff[lesserIndex];
        std::cout << "t: " << t << "\n";
        // Color Calculation
        GColor newColor = color[lesserIndex] + colorDiff[lesserIndex]*t;
        // GColor --> GPixel
        int a = (int) std::round(newColor.a * 255);
        int r = (int) std::round(newColor.r * 255 * newColor.a);
        int g = (int) std::round(newColor.g * 255 * newColor.a);
        int b = (int) std::round(newColor.b * 255 * newColor.a);
        row[i] = GPixel_PackARGB(a, r, g, b);

        // Increment x before moving onto the next row pixel
        px += dx;
      }
    }
  }

private:
  GMatrix shaderMatrix;
  GMatrix contextMatrix;
  GMatrix invMatrix;
  std::vector<GColor> color;
  std::vector<float> position;
  std::vector<GColor> colorDiff;
  std::vector<float> positionDiff;
  const int num;
  bool opaque;

  GMatrix getShaderMatrix(GPoint p0, GPoint p1) {
    float dx = p1.x - p0.x;
    float dy = p1.y - p0.y;
    GMatrix matrix = GMatrix(dx, -dy, p0.x, dy, dx, p0.y);
    return *matrix.invert();
  }

  int findPosIndex(float ix) {
    int left = num;
    int right = num / 2;
    while (right > left + 1) {
      int mid = left + (right - left) / 2;
      if (position[mid] > ix) {
        right = mid;
      } else {
        left = mid;
      }
    }
    std::cout << "Index, Position: " << left << ", " << position[left] << "\n";
    return left;
  }
};

#endif //GSHADER_POSGRADIENT_H
