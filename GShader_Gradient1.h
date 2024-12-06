/*
 *  Copyright 2024 Christine Hu
 */

#ifndef GSHADER_GRADIENT1_H
#define GSHADER_GRADIENT1_H

#include "include/GShader.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"


class GShader_Gradient1 : public GShader {
public:
    GShader_Gradient1(GPoint p0, GPoint p1, const GColor colors[], int count) {
        GColor tempColor = colors[0];
        int a = (int) std::round(tempColor.a * 255);
        int r = (int) std::round(tempColor.r * 255 * tempColor.a);
        int g = (int) std::round(tempColor.g * 255 * tempColor.a);
        int b = (int) std::round(tempColor.b * 255 * tempColor.a);
        colorPixel = GPixel_PackARGB(a, r, g, b);
        opaque = (a == 255);
    }

    virtual bool isOpaque() override;

    // Returns false if the inverse matrix does not exist
    virtual bool setContext(const GMatrix& ctm) override;

    virtual void shadeRow(int x, int y, int count, GPixel row[]) override;

private:
    GPixel colorPixel;
    bool opaque;
};



#endif //GSHADER_GRADIENT1_H
