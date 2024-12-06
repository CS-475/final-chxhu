/*
 *  Copyright 2024 Christine Hu
 */

#include "GShader_Bitmap.h"


bool GShader_Bitmap::isOpaque() {
  return sBitmap.isOpaque();
}

bool GShader_Bitmap::setContext(const GMatrix& ctm) {
  if (contextMatrix == ctm) {
    return true;
  }

  GMatrix midpointMatrix = ctm * shaderMatrix;
  nonstd::optional<GMatrix> invMatrixPointer = midpointMatrix.invert();

  if (invMatrixPointer.has_value()) {
    contextMatrix = ctm;
    invMatrix = *invMatrixPointer;
    return true;
  }
  return false;
}

void GShader_Bitmap::shadeRow_kClamp(float a, float b, float px, float py, int count, GPixel row[]) {
  int ix, iy;
  float maxWidth = width - 1.f;
  float maxHeight = height - 1.f;

  if (b == 0.0f) {  // No image rotation --> no need to recalculate py, iy.
    // Clamp and floor y
    iy = (int) std::floor(fmin(fmax(py, 0.f), maxHeight));

    // Loop through row
    for (int i = 0; i < count; i++) {
      // Clamp and floor x
      ix = (int) std::floor(fmin(fmax(px, 0.f), maxWidth));
      // Fetch pixel from bitmap
      row[i] = *sBitmap.getAddr(ix, iy);
      // Increment px
      px += a;
    }
  } else {  // Image rotation.
    // Loop through row
    for (int i = 0; i < count; i++) {
      // Clamp and floor x, y
      ix = (int) std::floor(fmin(fmax(px, 0.f), maxWidth));
      iy = (int) std::floor(fmin(fmax(py, 0.f), maxHeight));
      // Fetch pixel from bitmap
      row[i] = *sBitmap.getAddr(ix, iy);
      // Increment px
      px += a;
      py += b;
    }
  }
}

void GShader_Bitmap::shadeRow_kRepeat(float a, float b, float px, float py, int count, GPixel row[]) {
  int ix, iy;
  int maxWidth = sBitmap.width() - 1;
  int maxHeight = sBitmap.height() - 1;

  if (b == 0.0f) {  // No image rotation --> no need to recalculate py, iy.
    // Clamp and floor y
    iy = (int) std::floor(abs(fmod(py, height)));
    if (py < 0.f) {
      iy = maxHeight - iy;
    }

    // Loop through row
    for (int i = 0; i < count; i++) {
      // Clamp and floor x
      ix = (int) std::floor(abs(fmod(px, width)));
      if (px < 0.f) {
        ix = maxWidth - ix;
      }
      // Fetch pixel from bitmap
      row[i] = *sBitmap.getAddr(ix, iy);
      // Increment px
      px += a;
    }
  } else {  // Image rotation.
    // Loop through row
    for (int i = 0; i < count; i++) {
      // Clamp and floor x, y
      ix = (int) std::floor(abs(fmod(px, width)));
      iy = (int) std::floor(abs(fmod(py, height)));
      if (px < 0.f) {
        ix = maxWidth - ix;
      }
      if (py < 0.f) {
        iy = maxHeight - iy;
      }
      // Fetch pixel from bitmap
      row[i] = *sBitmap.getAddr(ix, iy);
      // Increment px
      px += a;
      py += b;
    }
  }
}

void GShader_Bitmap::shadeRow_kMirror(float a, float b, float px, float py, int count, GPixel row[]) {
  int ix, iy;
  float maxWidth = width - 1.f;
  float maxHeight = height - 1.f;

  if (b == 0.0f) {  // No image rotation --> no need to recalculate py, iy.
    float ydecimal = fmod(py, height);
    float yint = py - ydecimal;
    bool yintOdd = ((int) fmod(yint, 2)) != 0;
    if ((yintOdd && py > 0.f) || (!yintOdd && py < 0.f)) {
      ydecimal = maxHeight - ydecimal;
    }

    iy = (int) std::floor(fmin(fmax(ydecimal, 0.f), maxHeight));

    // Loop through row
    for (int i = 0; i < count; i++) {
      // Clamp and floor x
      float xdecimal = fmod(px, width);
      float xint = px - xdecimal;
      bool xintOdd = ((int) fmod(xint, 2)) != 0;
      if ((xintOdd && px > 0.f) || (!xintOdd && px < 0.f)) {
        xdecimal = maxWidth - xdecimal;
      }
      ix = (int) std::floor(fmin(fmax(xdecimal, 0.f), maxWidth));

      // Fetch pixel from bitmap
      row[i] = *sBitmap.getAddr(ix, iy);
      // Increment px
      px += a;
    }
  } else {  // Image rotation.
    // Loop through row
    for (int i = 0; i < count; i++) {
      // Clamp and floor x, y
      float xdecimal = fmod(px, width);
      float xint = px - xdecimal;
      bool xintOdd = ((int) fmod(xint, 2)) != 0;
      if ((xintOdd && px > 0.f) || (!xintOdd && px < 0.f)) {
        xdecimal = maxWidth - 1 - xdecimal;
      }
      ix = (int) std::floor(fmin(fmax(xdecimal, 0.f), maxWidth));

      float ydecimal = fmod(py, height);
      float yint = py - ydecimal;
      bool yintOdd = ((int) fmod(yint, 2)) != 0;
      if ((yintOdd && py > 0.f) || (!yintOdd && py < 0.f)) {
        ydecimal = maxHeight - ydecimal;
      }

      iy = (int) std::floor(fmin(fmax(ydecimal, 0.f), maxHeight));

      // Fetch pixel from bitmap
      row[i] = *sBitmap.getAddr(ix, iy);
      // Increment px
      px += a;
      py += b;
    }
  }
}

void GShader_Bitmap::shadeRow(int x, int y, int count, GPixel row[]) {
  float a = invMatrix[0];
  float b = invMatrix[1];
  float px = a*(x + 0.5f) + invMatrix[2] * (y + 0.5f) + invMatrix[4];
  float py = b*(x + 0.5f) + invMatrix[3] * (y + 0.5f) + invMatrix[5];

  // Note: Currently, the only differences between each function is the clamp method.
  switch (tileMode) {
    case 0: // kClamp
      shadeRow_kClamp(a, b, px, py, count, row);
      break;
    case 1: // kRepeat
      shadeRow_kRepeat(a, b, px, py, count, row);
      break;
    case 2: // kMirror
      shadeRow_kMirror(a, b, px, py, count, row);
      break;
  }
}