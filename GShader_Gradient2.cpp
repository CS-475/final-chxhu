/*
 *  Copyright 2024 Christine Hu
 */

#include "GShader_Gradient2.h"

bool GShader_Gradient2::isOpaque() {
  return opaque;
}

bool GShader_Gradient2::setContext(const GMatrix& ctm) {
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

void GShader_Gradient2::shadeRow_kClamp(float x_f, float y_f, int count, GPixel row[]) {
  // tileMode if-then
  if (opaque) {
    for (int i = 0; i < count; i++) {
      // Convert to [0, 1) gradient plane
      float px = invMatrix[0]*x_f + invMatrix[2]*y_f + invMatrix[4];
      // Clamp
      px = fmin(fmax(px, 0.f), 0.9999f);
      // Color Calculation
      GColor newColor = color0 + colorDiff*px;
      // GColor --> GPixel
      int r = (int) std::round(newColor.r * 255);
      int g = (int) std::round(newColor.g * 255);
      int b = (int) std::round(newColor.b * 255);
      row[i] = GPixel_PackARGB(255, r, g, b);

      // Increment x before moving onto the next row pixel
      x_f += 1.f;
    }
  } else {
    for (int i = 0; i < count; i++) {
      // Convert to [0, 1) gradient plane
      float px = invMatrix[0]*x_f + invMatrix[2]*y_f + invMatrix[4];
      // Clamp
      px = fmin(fmax(px, 0.f), 0.9999f);
      // Color Calculation
      GColor newColor = color0 + colorDiff*px;
      // GColor --> GPixel
      int a = (int) std::round(newColor.a * 255);
      int r = (int) std::round(newColor.r * 255 * newColor.a);
      int g = (int) std::round(newColor.g * 255 * newColor.a);
      int b = (int) std::round(newColor.b * 255 * newColor.a);
      row[i] = GPixel_PackARGB(a, r, g, b);

      // Increment x before moving onto the next row pixel
      x_f += 1.f;
    }
  }
}

void GShader_Gradient2::shadeRow_kRepeat(float x_f, float y_f, int count, GPixel row[]) {
  // tileMode if-then
  if (opaque) {
    for (int i = 0; i < count; i++) {
      // Convert to [0, 1) gradient plane
      float px = invMatrix[0]*x_f + invMatrix[2]*y_f + invMatrix[4];
      // Clamp
      px = abs(fmodf(px, 1.f));
      // Color Calculation
      GColor newColor = color0 + colorDiff*px;
      // GColor --> GPixel
      int r = (int) std::round(newColor.r * 255);
      int g = (int) std::round(newColor.g * 255);
      int b = (int) std::round(newColor.b * 255);
      row[i] = GPixel_PackARGB(255, r, g, b);

      // Increment x before moving onto the next row pixel
      x_f += 1.f;
    }
  } else {
    for (int i = 0; i < count; i++) {
      // Convert to [0, 1) gradient plane
      float px = invMatrix[0]*x_f + invMatrix[2]*y_f + invMatrix[4];
      // Clamp
      px = abs(fmodf(px, 1.f));
      // Color Calculation
      GColor newColor = color0 + colorDiff*px;
      // GColor --> GPixel
      int a = (int) std::round(newColor.a * 255);
      int r = (int) std::round(newColor.r * 255 * newColor.a);
      int g = (int) std::round(newColor.g * 255 * newColor.a);
      int b = (int) std::round(newColor.b * 255 * newColor.a);
      row[i] = GPixel_PackARGB(a, r, g, b);

      // Increment x before moving onto the next row pixel
      x_f += 1.f;
    }
  }
}

void GShader_Gradient2::shadeRow_kMirror(float x_f, float y_f, int count, GPixel row[]) {
  // tileMode if-then
  if (opaque) {
    for (int i = 0; i < count; i++) {
      // Convert to [0, 1) gradient plane
      float px = invMatrix[0]*x_f + invMatrix[2]*y_f + invMatrix[4];
      // Clamp. If the iteration is odd, invert px.
      float floorx = std::floor(px);
      px = px - floorx;
      if (abs(fmodf(floorx, 2.f)) == 1.f) {
        px = 1.f - px;
      }
      // Color Calculation
      GColor newColor = color0 + colorDiff*px;
      // GColor --> GPixel
      int r = (int) std::round(newColor.r * 255);
      int g = (int) std::round(newColor.g * 255);
      int b = (int) std::round(newColor.b * 255);
      row[i] = GPixel_PackARGB(255, r, g, b);

      // Increment x before moving onto the next row pixel
      x_f += 1.f;
    }
  } else {
    for (int i = 0; i < count; i++) {
      // Convert to [0, 1) gradient plane
      float px = invMatrix[0]*x_f + invMatrix[2]*y_f + invMatrix[4];
      // Clamp. If the iteration is odd, invert px.
      float floorx = std::floor(px);
      px = px - floorx;
      if (abs(fmodf(floorx, 2.f)) == 1.f) {
        px = 1.f - px;
      }
      // Color Calculation
      GColor newColor = color0 + colorDiff*px;
      // GColor --> GPixel
      int a = (int) std::round(newColor.a * 255);
      int r = (int) std::round(newColor.r * 255 * newColor.a);
      int g = (int) std::round(newColor.g * 255 * newColor.a);
      int b = (int) std::round(newColor.b * 255 * newColor.a);
      row[i] = GPixel_PackARGB(a, r, g, b);

      // Increment x before moving onto the next row pixel
      x_f += 1.f;
    }
  }
}

void GShader_Gradient2::shadeRow(int x, int y, int count, GPixel row[]) {
  float x_f = x + 0.5f;
  float y_f = y + 0.5f;

  // Note: Currently, the only differences between each function is the clamp method.
  switch (tileMode) {
    case 0: // kClamp
      shadeRow_kClamp(x_f, y_f, count, row);
      break;
    case 1: // kRepeat
      shadeRow_kRepeat(x_f, y_f, count, row);
      break;
    case 2: // kMirror
      shadeRow_kMirror(x_f, y_f, count, row);
      break;
  }
}