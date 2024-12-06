/*
 *  Copyright 2024 Christine Hu
 */

#include "GShader_Gradient1.h"
bool GShader_Gradient1::isOpaque() {
    return opaque;
}

bool GShader_Gradient1::setContext(const GMatrix& ctm) {
    return true;
}

void GShader_Gradient1::shadeRow(int x, int y, int count, GPixel row[]) {
    for (int i = 0; i < count; i++) {
        row[i] = colorPixel;
    }
}