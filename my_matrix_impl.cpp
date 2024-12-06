/*
 *  Copyright 2024 Christine Hu
 */

#include "include/GMatrix.h"

GMatrix::GMatrix() {
  fMat[0] = 1;    fMat[2] = 0;    fMat[4] = 0;
  fMat[1] = 0;    fMat[3] = 1;    fMat[5] = 0;
}

GMatrix GMatrix::Translate(float tx, float ty) {
  //std::cout << "Translate";
  return GMatrix(1, 0, tx, 0, 1, ty);
}
GMatrix GMatrix::Scale(float sx, float sy) {
  //std::cout << "Scale";
  return GMatrix(sx, 0, 0, 0, sy, 0);
}
GMatrix GMatrix::Rotate(float radians) {
  //std::cout << "Rotate";
  return GMatrix(cos(radians), -sin(radians), 0, sin(radians), cos(radians), 0);
}

GMatrix GMatrix::Concat(const GMatrix& a, const GMatrix& b) {
  // new_a: a_a * b_a + a_c * b_b
  float new_a = a.fMat[0] * b.fMat[0] + a.fMat[2] * b.fMat[1];
  // new_b: a_b * b_a + a_d * b_b
  float new_b = a.fMat[1] * b.fMat[0] + a.fMat[3] * b.fMat[1];
  // new_c: a_a * b_c + a_c * b_d
  float new_c = a.fMat[0] * b.fMat[2] + a.fMat[2] * b.fMat[3];
  // new_d: a_b * b_c + a_d * b_d
  float new_d = a.fMat[1] * b.fMat[2] + a.fMat[3] * b.fMat[3];
  // new_e: a_a * b_e + a_c * b_f + a_e
  float new_e = a.fMat[0] * b.fMat[4] + a.fMat[2] * b.fMat[5] + a.fMat[4];
  // new_f: a_b * b_e + a_d * b_f + a_f
  float new_f = a.fMat[1] * b.fMat[4] + a.fMat[3] * b.fMat[5] + a.fMat[5];

  return GMatrix(new_a, new_c, new_e, new_b, new_d, new_f);
}

nonstd::optional<GMatrix> GMatrix::invert() const {
  // Note: As far as I can tell, inversion does work exactly like that of a 2x2 matrix, aside from the new e and f.
  float det = fMat[0] * fMat[3] - fMat[1] * fMat[2];
  if (det == 0) {
    return nonstd::nullopt;
  }

  float inv_det = 1 / det;
  float new_e = fMat[2] * fMat[5] - fMat[3] * fMat[4];
  float new_f = fMat[1] * fMat[4] - fMat[0] * fMat[5];

  return GMatrix(fMat[3] * inv_det, -fMat[2] * inv_det, new_e * inv_det, -fMat[1] * inv_det, fMat[0] * inv_det, new_f * inv_det);
}

void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const {
  // if GMatrix is the identity matrix, simply copy over everything from src to dst.
  if (fMat[0] == 1 && fMat[1] == 0 && fMat[2] == 0 && fMat[3] == 1 && fMat[4] == 0 && fMat[5] == 0) {
      for (int i = 0; i < count; i++) {
        dst[i].x = src[i].x;
        dst[i].y = src[i].y;
      }
  } else {
    float src_x, src_y;
    for (int i = 0; i < count; i++) {
      src_x = src[i].x;
      src_y = src[i].y;
      dst[i].x = fMat[0] * src_x + fMat[2] * src_y + fMat[4];
      dst[i].y = fMat[1] * src_x + fMat[3] * src_y + fMat[5];
    }
  }
}