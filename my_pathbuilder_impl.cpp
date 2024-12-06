/*
 *  Copyright 2024 Christine Hu
 */

#include "include/GPathBuilder.h"

void GPathBuilder::addRect(const GRect& rect, GPathDirection direction) {
  moveTo(rect.left, rect.top);
  if (direction == GPathDirection::kCW) {
    lineTo(rect.right, rect.top);
    lineTo(rect.right, rect.bottom);
    lineTo(rect.left, rect.bottom);
  } else {
    lineTo(rect.left, rect.bottom);
    lineTo(rect.right, rect.bottom);
    lineTo(rect.right, rect.top);
  }
}

void GPathBuilder::addPolygon(const GPoint pts[], int count) {
  moveTo(pts[0]);
  for (int i = 1; i < count; i++) {
    lineTo(pts[i]);
  }
}

// Creates cubic curves.
void GPathBuilder::addCircle(const GPoint center, float radius, GPathDirection direction) {
  float x = center.x;
  float y = center.y;

  // Calculate values
  float top = y - radius;
  float bottom = y + radius;
  float left = x - radius;
  float right = x + radius;
  float k = (radius * 4.f * (sqrtf(2.f) - 1.f)) / 3.f;

  // moveTo top point
  moveTo(x, top);

  // Add curves
  if (direction == GPathDirection::kCW) {
    // Upper-right curve
    cubicTo(x+k, top, right, y-k, right, y);
    // Lower-right curve
    cubicTo(right, y+k, x+k, bottom, x, bottom);
    // Lower-left curve
    cubicTo(x-k, bottom, left, y+k, left, y);
    // Upper-left curve
    cubicTo(left, y-k, x-k, top, x, top);
  } else {
    // Upper-left curve
    cubicTo(x-k, top, left, y-k, left, y);
    // Lower-left curve
    cubicTo(left, y+k, x-k, bottom, x, bottom);
    // Lower-right curve
    cubicTo(x+k, bottom, right, y+k, right, y);
    // Upper-right curve
    cubicTo(right, y-k, x+k, top, x, top);
  }
}
