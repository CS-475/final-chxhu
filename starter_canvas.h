/*
*  Copyright 2024 Christine Hu
 */

#ifndef _g_starter_canvas_h_
#define _g_starter_canvas_h_

#include "include/GCanvas.h"
#include "include/GRect.h"
#include "include/GColor.h"
#include "include/GBitmap.h"
#include "include/GPath.h"
#include <list>

class MyCanvas : public GCanvas {
public:
    MyCanvas(const GBitmap& device) : fDevice(device) {
      currentMatrix = GMatrix();
    }

    void clear(const GColor& color) override;

    virtual void drawRect(const GRect&, const GPaint&) override;
    virtual void drawConvexPolygon(const GPoint[], int count, const GPaint&) override;
    virtual void drawPath(const GPath& path, const GPaint&) override;
    virtual void drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[],
                              int count, const int indices[], const GPaint& paint) override;
    virtual void drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4],
                              int level, const GPaint&) override;

    // Managing the stack of Transform Matrices
    virtual void save() override;
    virtual void restore() override;
    virtual void concat(const GMatrix&) override;

private:
    // Note: we store a copy of the bitmap
    const GBitmap fDevice;

    // Add whatever other fields you need
    std::list<GMatrix> savedMatrices;
    GMatrix currentMatrix;
};

#endif
