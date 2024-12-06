/*
 *  Copyright 2024 Christine Hu
 */

#include "include/GFinal.h"

std::unique_ptr<GFinal> GCreateFinal() {
 return std::unique_ptr<GFinal>(new GFinal());
}