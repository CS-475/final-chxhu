/*
 *  Copyright 2024 Christine Hu
 */

#ifndef GSHADER_GRADIENT_H
#define GSHADER_GRADIENT_H

#include "include/GShader.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"


class GShader_Gradient : public GShader {
public:
    GShader_Gradient(GPoint p0, GPoint p1, const GColor colors[], int count, GTileMode tMode) : count(count) {
        shaderMatrix = getShaderMatrix(p0, p1);
        contextMatrix = GMatrix();
        invMatrix = shaderMatrix;
        color.assign(colors, colors + count);
        colorDiff.resize(count - 1);
        opaque = colors[0].a <= 0.9981f;
        for (int i=1; i<count; i++) {
            if (colors[i].a <= 0.9981f) {
                opaque = false;
            }
            colorDiff[i-1] = colors[i] - colors[i-1];
        }
        k = count - 1;
        tileMode = (int) tMode;
    }

    virtual bool isOpaque() override;

    // Returns false if the inverse matrix does not exist
    virtual bool setContext(const GMatrix& ctm) override;

    virtual void shadeRow(int x, int y, int count, GPixel row[]) override;

private:
    GMatrix shaderMatrix;
    GMatrix contextMatrix;
    GMatrix invMatrix;
    std::vector<GColor> color;
    std::vector<GColor> colorDiff;
    const int count;
    float k;
    bool opaque;
    int tileMode;

    GMatrix getShaderMatrix(GPoint p0, GPoint p1) {
        float dx = p1.x - p0.x;
        float dy = p1.y - p0.y;
        GMatrix matrix = GMatrix(dx, -dy, p0.x, dy, dx, p0.y);
        return *matrix.invert();
    }

    void shadeRow_kClamp(float x_f, float y_f, int count, GPixel row[]);
    void shadeRow_kRepeat(float x_f, float y_f, int count, GPixel row[]);
    void shadeRow_kMirror(float x_f, float y_f, int count, GPixel row[]);
};



#endif //GSHADER_GRADIENT_H
