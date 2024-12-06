/*
 *  Copyright 2024 Christine Hu
 */

#ifndef GSHADER_VORONOI_H
#define GSHADER_VORONOI_H

#include "include/GShader.h"
#include "include/GMatrix.h"

class GShader_Voronoi : public GShader {
public:
  GShader_Voronoi(const GPoint points[], const GColor colors[], int count) : count(count) {
    contextMatrix = GMatrix();
    invMatrix = GMatrix();
    point.assign(points, points + count);
    opaque = true;
    for (int i = 0; i < count; i++) {
      if (colors[i].a <= 0.9981f) {
        opaque = false;
      }
      int a = (int) std::round(colors[i].a * 255);
      int r = (int) std::round(colors[i].r * 255);
      int g = (int) std::round(colors[i].g * 255);
      int b = (int) std::round(colors[i].b * 255);
      pixel.push_back(GPixel_PackARGB(a, r, g, b));
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
      invMatrix = *invCTMPointer;
      return true;
    }
    return false;
  }

  virtual void shadeRow(int x, int y, int count, GPixel row[]) {
    float a = invMatrix[0];
    float b = invMatrix[1];
    float px = a*(x + 0.5f) + invMatrix[2] * (y + 0.5f) + invMatrix[4];
    float py = b*(x + 0.5f) + invMatrix[3] * (y + 0.5f) + invMatrix[5];

    // Note to self: px increments by a, py increments by b
    if (b == 0.0f) { // no need to recalculate py
      // Loop through row
      for (int i = 0; i < count; i++) {
        // Find closest color
        row[i] = findClosestColor(px, py);
        // Increment px
        px += a;
      }
    } else {
      // Loop through row
      for (int i = 0; i < count; i++) {
        // Fetch pixel from bitmap
        row[i] = findClosestColor(px, py);
        // Increment px
        px += a;
        py += b;
      }
    }
  }



private:
  GMatrix contextMatrix;
  GMatrix invMatrix;
  std::vector<GPoint> point;
  std::vector<GPixel> pixel;
  int count;
  bool opaque;

  GPixel findClosestColor(float x, float y) {
    int index = 0;
    float distance = findDistance(x, y, point[0]);
    for (int i=1; i<count; i++) {
      float newDistance = findDistance(x, y, point[i]);
      if (newDistance < distance) {
        index = i;
        distance = newDistance;
      }
    }
    return pixel[index];
  }

  float findDistance(float x1, float y1, GPoint p2) {
    return sqrt(pow(p2.x - x1, 2) + pow(p2.y - y1, 2));
  }
}

#endif //GSHADER_VORONOI_H
