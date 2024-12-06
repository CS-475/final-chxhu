/*
 *  Copyright 2024 Christine Hu
 */

#ifndef GFINAL_LIMITED_H
#define GFINAL_LIMITED_H

#include "include/GFinal.h"
#include "GShader_Gradient1.h"
#include "GShader_Voronoi.h"
#include "GShader_PosGradient.h"


class GFinal_Limited : public GFinal {
public:
    GFinal_Limited() {}

    virtual std::shared_ptr<GShader> createVoronoiShader(const GPoint points[],
                                                         const GColor colors[],
                                                         int count) {
        if (count == 1) {
            return std::make_shared<GShader_Gradient1>(GPoint(), GPoint(), colors, count);
        } else {
            return std::make_shared<GShader_Voronoi>(points, colors, count);
        }
    }

    virtual std::shared_ptr<GShader> createSweepGradient(GPoint center, float startRadians,
                                                         const GColor colors[], int count) {
        return nullptr;
    }

    virtual std::shared_ptr<GShader> createLinearPosGradient(GPoint p0, GPoint p1,
                                                             const GColor colors[],
                                                             const float pos[],
                                                             int count) {
        if (count == 1) {
            return std::make_shared<GShader_Gradient1>(p0, p1, colors, count);
        } else {
            return std::make_shared<GShader_PosGradient>(p0, p1, colors, pos, count);
        }
    }

    virtual std::shared_ptr<GShader> createColorMatrixShader(const GColorMatrix&,
                                                             GShader* realShader) {
        return nullptr;
    }

    virtual std::shared_ptr<GPath> strokePolygon(const GPoint[], int count, float width, bool isClosed) {
        return nullptr;
    }

    virtual void drawQuadraticCoons(GCanvas*, const GPoint pts[8], const GPoint tex[4],
                                    int level, const GPaint&) {}
};



#endif //GFINAL_LIMITED_H
