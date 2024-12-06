/*
 *  Copyright 2024 Christine Hu
 */

#ifndef TRISHADER_FACTORY_H
#define TRISHADER_FACTORY_H

#include "include/GShader.h"
#include "GShader_TriSticking.h"
#include "GShader_TriGradient.h"
#include "GShader_TriCompose.h"

std::shared_ptr<GShader> GCreateTriGradientShader(GPoint p0, GPoint p1, GPoint p2, GColor c0, GColor c1, GColor c2) {
    return std::make_shared<GShader_TriGradient>(p0, p1, p2, c0, c1, c2);
}

std::shared_ptr<GShader> GCreateTriStickingShader(GPoint p0, GPoint p1, GPoint p2, GPoint t0, GPoint t1, GPoint t2, GShader* bmShader) {
    return std::make_shared<GShader_TriSticking>(p0, p1, p2, t0, t1, t2, bmShader);
}

std::shared_ptr<GShader> GCreateTriComposeShader(GShader* gradientShader, GShader* stickingShader) {
    return std::make_shared<GShader_TriCompose>(gradientShader, stickingShader);
}

#endif //TRISHADER_FACTORY_H
