/*
 *  Copyright 2024 Christine Hu
 */

#include "include/GShader.h"
#include "GShader_Bitmap.h"
#include "GShader_Gradient.h"
#include "GShader_Gradient1.h"
#include "GShader_Gradient2.h"

std::shared_ptr<GShader> GCreateBitmapShader(const GBitmap& bitmap, const GMatrix& localMatrix, GTileMode tileMode) {
  return std::make_shared<GShader_Bitmap>(bitmap, localMatrix, tileMode);
}

std::shared_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor colors[], int count, GTileMode tileMode) {
  if (count == 1) {
    return std::make_shared<GShader_Gradient1>(p0, p1, colors, count);
  } else if (count == 2) {
    return std::make_shared<GShader_Gradient2>(p0, p1, colors, count, tileMode);
  }

  return std::make_shared<GShader_Gradient>(p0, p1, colors, count, tileMode);
}
