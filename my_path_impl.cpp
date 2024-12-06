/*
 *  Copyright 2024 Christine Hu
 */

#include "include/GPath.h"
#include "include/GPathBuilder.h"

GRect GPath::bounds() const {
  if (fPts.size() == 0) {
    return GRect::LTRB(0, 0, 0, 0);
  }

  /* This function goes through every move.
   * If kMove, fmin + fmax the current point's coordinates.
   * If there is a curve, find the tight bounds of the curve and fmin + fmax the last point's coordinates.
   * If there is a line, just do the latter.
   */

  GPoint pts[kMaxNextPoints];
  Edger edger(*this);

  GPoint p;
  float left, right;
  float top, bottom;
  bool firstLoop = true;

  while (auto v = edger.next(pts)) {
    if (firstLoop) {
      p = pts[0];
      left = p.x;
      right = p.x;
      top = p.y;
      bottom = p.y;
      firstLoop = false;
    }

    if (v.value() == GPathVerb::kMove) {
      p = pts[0];
    } else if (v.value() == GPathVerb::kLine) {
      p = pts[1];
    } else if (v.value() == GPathVerb::kQuad) {
      GPoint a = pts[0];
      GPoint b = pts[1];
      p = pts[2];

      // Equation: n*t^2 + 2*o*t + a
      float n_x = a.x - 2.f*b.x + p.x;
      float o_x = -a.x + b.x;
      float n_y = a.y - 2.f*b.y + p.y;
      float o_y = -a.y + b.y;
      // Find middle T for x-values
      if (n_x != 0) {
        float middleT = -(o_x / n_x);
        if (middleT > 0 && middleT < 1) {
          float middleX = ((n_x*middleT + 2*o_x)*middleT + a.x);
       	  left = fmin(left, middleX);
    	  right = fmax(right, middleX);
        }
      }
      // Find middle T for y-values
      if (n_y != 0) {
        float middleT = -(o_y / n_y);
        if (middleT > 0 && middleT < 1) {
          float middleY = ((n_y*middleT + 2*o_y)*middleT + a.y);
    	  top = fmin(top, middleY);
    	  bottom = fmax(bottom, middleY);
        }
      }
    } else if (v.value() == GPathVerb::kCubic) {
      GPoint a = pts[0];
      GPoint b = pts[1];
      GPoint c = pts[2];
      p = pts[3];

      // Equation: m*t^3 + 3*n*t^2 + 3*o*t + a
      float m_x = -a.x + 3.f*b.x - 3.f*c.x + p.x;
      float n_x = a.x - 2*b.x + c.x;
      float o_x = -a.x + b.x;

      float m_y = -a.y + 3.f*b.y - 3.f*c.y + p.y;
      float n_y = a.y - 2*b.y + c.y;
      float o_y = -a.y + b.y;

      // Find middle t for x-values
      if (m_x != 0) {
        float negN = -1*(n_x / m_x);
        float negSqrt = sqrt(pow(n_x, 2.f) - m_x*o_x) / m_x;
        // Quadratic Equation, +
        float middleT = negN + negSqrt;
        if (middleT > 0.f && middleT < 1.f) {
          float middleX = ((m_x*middleT + 3*n_x)*middleT + 3*o_x)*middleT + a.x;
       	  left = fmin(left, middleX);
    	  right = fmax(right, middleX);
      	}
        // Quadratic Equation, -
        middleT = negN - negSqrt;
        if (middleT > 0.f && middleT < 1.f) {
          float middleX = ((m_x*middleT + 3*n_x)*middleT + 3*o_x)*middleT + a.x;
       	  left = fmin(left, middleX);
    	  right = fmax(right, middleX);
      	}
      } else if (n_x != 0) {
        float middleT = -(o_x / (2*n_x));
        if (middleT > 0.f && middleT < 1.f) {
          float middleX = ((m_x*middleT + 3*n_x)*middleT + 3*o_x)*middleT + a.x;
       	  left = fmin(left, middleX);
    	  right = fmax(right, middleX);
      	}
      }

      // Find middle t for y-values
      if (m_y != 0) {
        float negN = -1*(n_y / m_y);
        float negSqrt = sqrt(pow(n_y, 2.f) - m_y*o_y) / m_y;
        // Quadratic Equation, +
        float middleT = negN + negSqrt;
        if (middleT > 0 && middleT < 1) {
          float middleY = ((m_y*middleT + 3*n_y)*middleT + 3*o_y)*middleT + a.y;
    	  top = fmin(top, middleY);
    	  bottom = fmax(bottom, middleY);
      	}
        // Quadratic Equation, -
        middleT = negN - negSqrt;
        if (middleT > 0 && middleT < 1) {
          float middleY = ((m_y*middleT + 3*n_y)*middleT + 3*o_y)*middleT + a.y;
    	  top = fmin(top, middleY);
    	  bottom = fmax(bottom, middleY);
      	}
      } else if (n_y != 0) {
        float middleT = -(o_y / (2*n_y));
        if (middleT > 0 && middleT < 1) {
          float middleY = ((m_y*middleT + 3*n_y)*middleT + 3*o_y)*middleT + a.y;
    	  top = fmin(top, middleY);
    	  bottom = fmax(bottom, middleY);
      	}
	  }
    }

    // Consider the current point.
    left = fmin(left, p.x);
    right = fmax(right, p.x);
    top = fmin(top, p.y);
    bottom = fmax(bottom, p.y);
  }

  return GRect::LTRB(left, top, right, bottom);
}

GPoint getPointAtT(GPoint a, GPoint b, float t, float invT) {
  return invT*a + t*b;
}

void GPath::ChopQuadAt(const GPoint src[3], GPoint dst[5], float t) {
  // Transfer endpoints A,C to dst
  dst[0] = src[0];
  dst[4] = src[2];

  float invT = 1 - t;

  // Find AB, BC at t with (1-t)G + tH given G -> H
  dst[1] = getPointAtT(src[0], src[1], t, invT);
  dst[3] = getPointAtT(src[1], src[2], t, invT);

  // Find f(t)
  dst[2] = getPointAtT(dst[1], dst[3], t, invT);
}

void GPath::ChopCubicAt(const GPoint src[4], GPoint dst[7], float t) {
  // Transfer endpoints A,D to dst
  dst[0] = src[0];
  dst[6] = src[3];

  float invT = 1 - t;

  // Find AB, CD at t
  dst[1] = getPointAtT(src[0], src[1], t, invT);
  dst[5] = getPointAtT(src[2], src[3], t, invT);

  // Find C at t
  GPoint c = getPointAtT(src[1], src[2], t, invT);

  // Find AB-C, B-CD
  dst[2] = getPointAtT(dst[1], c, t, invT);
  dst[4] = getPointAtT(c, dst[5], t, invT);

  // Find ABC-BCD
  dst[3] = getPointAtT(dst[2], dst[4], t, invT);
}