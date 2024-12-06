/*
 *  Copyright 2024 Christine Hu
 */

#include "GShader_TriGradient.h"

bool GShader_TriGradient::isOpaque() {
  return opaque;
}

bool GShader_TriGradient::setContext(const GMatrix& ctm) {
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

void GShader_TriGradient::shadeRow(int x, int y, int count, GPixel row[]) {
  // Note: Assumes that px and py are always within the (0, 1) right triangle.
  float x_f = x + 0.5f;
  float y_f = y + 0.5f;

  float dx = invMatrix[0];
  float dy = invMatrix[1];
  float ix = dx*x_f + invMatrix[2]*y_f + invMatrix[4];
  float iy = dy*x_f + invMatrix[3]*y_f + invMatrix[5];
  GColor newColor = dColor0 * ix + dColor1 * iy + color2;
  GColor dColor = dColor0 * dx + dColor1 * dy;

  if (opaque) {
    for (int i = 0; i < count; i++) {
      // clamp, GColor --> GPixel
      int r = (int) std::round(fmin(fmax(newColor.r, 0.f), 0.9999f) * 255);
      int g = (int) std::round(fmin(fmax(newColor.g, 0.f), 0.9999f) * 255);
      int b = (int) std::round(fmin(fmax(newColor.b, 0.f), 0.9999f) * 255);
      row[i] = GPixel_PackARGB(255, r, g, b);

      // Increment newColor before moving onto the next row pixel
      newColor += dColor;
    }
  } else {
    for (int i = 0; i < count; i++) {
      // clamp, GColor --> GPixel
      float clamped_a = fmin(fmax(newColor.a, 0.f), 0.9999f);
      int a = (int) std::round(clamped_a * 255);
      int r = (int) std::round(fmin(fmax(newColor.r, 0.f), 0.9999f) * clamped_a * 255);
      int g = (int) std::round(fmin(fmax(newColor.g, 0.f), 0.9999f) * clamped_a * 255);
      int b = (int) std::round(fmin(fmax(newColor.b, 0.f), 0.9999f) * clamped_a * 255);
      row[i] = GPixel_PackARGB(a, r, g, b);

      // Increment newColor before moving onto the next row pixel
      newColor += dColor;
    }
  }
}