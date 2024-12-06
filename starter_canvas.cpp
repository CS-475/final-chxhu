/*
 *  Copyright 2024 Christine Hu
 */

#include "starter_canvas.h"
#include "include/GShader.h"
#include <algorithm>
#include "include/GMath.h"
#include "TriShader_Factory.h"

using BlendFunc = GPixel (GPixel, int, int, int, int);

struct Edge {
    float mx, bx;
    int y0_round, y1_round;

    bool operator<(Edge e) const {
        return (y0_round < e.y0_round);
    }

    int findX(int y) const {
        return GRoundToInt(mx * y + bx);
    }
};

struct PathEdge {
    float mx, bx;
    int y0_round, y1_round;
    int direction; // down = 1; up = -1
    int row_x;

    bool operator<(PathEdge e) const {
        return (y0_round < e.y0_round);
    }

    int findX(int y) const {
        return GRoundToInt(mx * y + bx);
    }

    bool isValid(int y) const {
        return (y >= y0_round && y <= y1_round);
    }
};


GPixel GColorToGPixel(GColor color) {
    int a = (int) std::round(color.a * 255);
    int r = (int) std::round(color.r * 255 * color.a);
    int g = (int) std::round(color.g * 255 * color.a);
    int b = (int) std::round(color.b * 255 * color.a);
    return GPixel_PackARGB(a, r, g, b);
}


// BLEND MODE FUNCTIONS
GPixel SrcOver(GPixel dest, int a, int r, int g, int b) {
    int reverseSa = 255 - a;

	int final_a = a + (int) std::round(GPixel_GetA(dest) * reverseSa / 255);
    if (final_a > 255) {
        final_a = 255;
    }
    int final_r = r + (int) std::round(GPixel_GetR(dest) * reverseSa / 255);
	int final_g = g + (int) std::round(GPixel_GetG(dest) * reverseSa / 255);
	int final_b = b + (int) std::round(GPixel_GetB(dest) * reverseSa / 255);

    return GPixel_PackARGB(final_a, final_r, final_g, final_b);
}

GPixel DstOver(GPixel dest, int a, int r, int g, int b) {
    int dest_a = GPixel_GetA(dest);
    int reverseDa = 255 - dest_a;

	int final_a = dest_a + (int) std::round(a * reverseDa / 255);
    if (final_a > 255) {
        final_a = 255;
    }
    int final_r = GPixel_GetR(dest) + (int) std::round(r * reverseDa / 255);
	int final_g = GPixel_GetG(dest) + (int) std::round(g * reverseDa / 255);
	int final_b = GPixel_GetB(dest) + (int) std::round(b * reverseDa / 255);

    return GPixel_PackARGB(final_a, final_r, final_g, final_b);
}

GPixel SrcIn(GPixel dest, int a, int r, int g, int b) {
    int dest_a = GPixel_GetA(dest);

	int final_a = (int) std::round(a * dest_a / 255);
    int final_r = (int) std::round(r * dest_a / 255);
	int final_g = (int) std::round(g * dest_a / 255);
	int final_b = (int) std::round(b * dest_a / 255);

    return GPixel_PackARGB(final_a, final_r, final_g, final_b);
}

GPixel DstIn(GPixel dest, int a, int r, int g, int b) {
    int final_a = (int) std::round(GPixel_GetA(dest) * a / 255);
    int final_r = (int) std::round(GPixel_GetR(dest) * a / 255);
	int final_g = (int) std::round(GPixel_GetG(dest) * a / 255);
	int final_b = (int) std::round(GPixel_GetB(dest) * a / 255);

    return GPixel_PackARGB(final_a, final_r, final_g, final_b);
}

GPixel SrcOut(GPixel dest, int a, int r, int g, int b) {
    int reverseDa = 255 - GPixel_GetA(dest);

	int final_a = (int) std::round(a * reverseDa / 255);
    int final_r = (int) std::round(r * reverseDa / 255);
	int final_g = (int) std::round(g * reverseDa / 255);
	int final_b = (int) std::round(b * reverseDa / 255);

    return GPixel_PackARGB(final_a, final_r, final_g, final_b);
}

GPixel DstOut(GPixel dest, int a, int r, int g, int b) {
    int reverseSa = 255 - a;

	int final_a = (int) std::round(GPixel_GetA(dest) * reverseSa / 255);
    int final_r = (int) std::round(GPixel_GetR(dest) * reverseSa / 255);
	int final_g = (int) std::round(GPixel_GetG(dest) * reverseSa / 255);
	int final_b = (int) std::round(GPixel_GetB(dest) * reverseSa / 255);

    return GPixel_PackARGB(final_a, final_r, final_g, final_b);
}

GPixel SrcATop(GPixel dest, int a, int r, int g, int b) {
    int dest_a = GPixel_GetA(dest);
    int reverseSa = 255 - a;

	int final_a = (int) std::round((a * dest_a + dest_a * reverseSa) / 255);
    if (final_a > 255) {
        final_a = 255;
    }
    int final_r = (int) std::round((r * dest_a + GPixel_GetR(dest) * reverseSa) / 255);
	int final_g = (int) std::round((g * dest_a + GPixel_GetG(dest) * reverseSa) / 255);
	int final_b = (int) std::round((b * dest_a + GPixel_GetB(dest) * reverseSa) / 255);

    return GPixel_PackARGB(final_a, final_r, final_g, final_b);
}

GPixel DstATop(GPixel dest, int a, int r, int g, int b) {
    int reverseDa = 255 - GPixel_GetA(dest);

    int final_a = (int) std::round((GPixel_GetA(dest) * a + a * reverseDa) / 255);
    int final_r = (int) std::round((GPixel_GetR(dest) * a + r * reverseDa) / 255);
	int final_g = (int) std::round((GPixel_GetG(dest) * a + g * reverseDa) / 255);
	int final_b = (int) std::round((GPixel_GetB(dest) * a + b * reverseDa) / 255);

	return GPixel_PackARGB(final_a, final_r, final_g, final_b);
}

GPixel Xor(GPixel dest, int a, int r, int g, int b) {
    int reverseSa = 255 - a;
    int reverseDa = 255 - GPixel_GetA(dest);

	int final_a = (int) std::round((GPixel_GetA(dest) * reverseSa + a * reverseDa) / 255);
    int final_r = (int) std::round((GPixel_GetR(dest) * reverseSa + r * reverseDa) / 255);
	int final_g = (int) std::round((GPixel_GetG(dest) * reverseSa + g * reverseDa) / 255);
	int final_b = (int) std::round((GPixel_GetB(dest) * reverseSa + b * reverseDa) / 255);

    return GPixel_PackARGB(final_a, final_r, final_g, final_b);

}


// BLEND MODE SELECTION FUNCTIONS
/*
 * Blend Mode selection for GColor
 */
GBlendMode simplifyBlendMode(GBlendMode mode, float src_a) {
    const int modeInt = (int) mode;
    switch (modeInt) {
        //kSrc
        case 1:
			if (src_a == 0.f) {
                return GBlendMode::kClear;
            }
            break;

        //kSrcOver
        case 3:
            if (src_a == 0.f) {
                return GBlendMode::kDst;
            } else if (src_a == 1.f) {
                return GBlendMode::kSrc;
            }
            break;

        //kDstOver
        case 4:
            if (src_a == 0.f) {
                return GBlendMode::kDst;
            }
            break;

        //kSrcIn
        case 5:
            if (src_a == 0.f) {
                return GBlendMode::kClear;
            }
            break;

        //kDstIn
        case 6:
            if (src_a == 0.f) {
                return GBlendMode::kClear;
            } else if (src_a == 1.f) {
                return GBlendMode::kDst;
            }
            break;

        //kSrcOut
        case 7:
            if (src_a == 0.f) {
                return GBlendMode::kClear;
            }
            break;

        //kDstOut
        case 8:
            if (src_a == 0.f) {
                return GBlendMode::kDst;
            } else if (src_a == 1.f) {
                return GBlendMode::kClear;
            }
            break;

        //kSrcATop
        case 9:
            if (src_a == 0.f) {
                return GBlendMode::kDst;
            } else if (src_a == 1.f) {
                return GBlendMode::kSrcIn;
            }
            break;

        //kDstATop
        case 10:
            if (src_a == 0.f) {
                return GBlendMode::kClear;
            } else if (src_a == 1.f) {
                return GBlendMode::kDstOver;
            }
            break;

        //kXor
        case 11:
            if (src_a == 0.f) {
                return GBlendMode::kDst;
            } else if (src_a == 1.f) {
                return GBlendMode::kSrcOut;
            }
            break;
    }

    return mode;
}

/*
 * Blend Mode selection for GShader; derived from GColor selection function.
 */
GBlendMode simplifyBlendMode(GBlendMode mode, bool isOpaque) {
    const int modeInt = (int) mode;
    if (isOpaque) {
        switch (modeInt) {
            //kSrcOver
            case 3:
                return GBlendMode::kSrc;

            //kDstIn
            case 6:
                return GBlendMode::kDst;

            //kDstOut
            case 8:
                return GBlendMode::kClear;

            //kSrcATop
            case 9:
                return GBlendMode::kSrcIn;

            //kDstATop
            case 10:
                return GBlendMode::kDstOver;

            //kXor
            case 11:
                return GBlendMode::kSrcOut;
        }
    }

    return mode;
}

BlendFunc* getBlendFunc(GBlendMode blendMode) {
    const int modeInt = (int) blendMode;
    switch (modeInt) {
        case 3:
            return SrcOver;
        case 4:
            return DstOver;
        case 5:
            return SrcIn;
        case 6:
            return DstIn;
        case 7:
            return SrcOut;
        case 8:
            return DstOut;
        case 9:
            return SrcATop;
        case 10:
            return DstATop;
        case 11:
            return Xor;
        default:
            return NULL;
    }
}


// POLYGON FUNCTIONS
/**
 * Given two coordinate pairs (a1, b1) and (a2, b2), find the corresponding new_a for (new_a, new_b)
 */
float findNewA1( float a1, float a2, float b1, float b2, float new_b ) {
    return a1 + ((new_b - b1) / (b2 - b1)) * (a2 - a1);
}

Edge makeEdge(float x0, float y0, float x1, float y1) {
    // Catch irrelevant / vertical lines
    if (std::round(y0) >= std::round(y1)) {
        return {0.f, 0.f, 0, 0};
    } else if (x0 == x1) {
        return {0, x0, GRoundToInt(y0), GRoundToInt(y1)};
    }

    // Calculate mx, bx; Assumes that (newx0, newy0) is the higher point
    float mx = (x1 - x0) / (y1 - y0);
    float bx = x0 - mx * y0;

    return {mx, bx, GRoundToInt(y0), GRoundToInt(y1)};
}

GPoint makePoint(float x, float y) {
    return {x, y};
}



// PATH FUNCTIONS
/**
 * Calls makeEdge and turns Edge -> PathEdge
 */
PathEdge makePathEdge(float x0, float y0, float x1, float y1, int direction, int canvasRight) {
    Edge e = makeEdge(x0, y0, x1, y1);

    // To-do: determine potential out-of-bounds due to y rounding in main edge loop instead
    float new_x0 = e.mx * e.y0_round + e.bx;
    float new_x1 = e.mx * e.y1_round + e.bx;
    if (new_x0 < 0 || new_x0 > canvasRight) {
        e.y0_round++;
    }
    if (new_x1 < 0 || new_x1 > canvasRight) {
        e.y1_round--;
    }
    return {e.mx, e.bx, e.y0_round, e.y1_round, direction, 0};
}

/**
 * Note: Since edges is a linked list, binary search is not possible.
 *       If another list-type exists with built-in sort, switch to that.
 *       Alternatively, create an array of iterators that can function as a binary search?
 * Returns int: The new k (after deleting edges)
 */
int sortFirstK(std::list<PathEdge> &edges, int k, int y) {
    std::list<PathEdge>::iterator it = edges.begin();
    int last_x = it->findX(y);
    it->row_x = last_x;
    it++;
    for (int i = 1; i < k; i++) {
        int current_x = it->findX(y);
        it->row_x = current_x;
        // Insertion Sort: If the current x is to the left of the last x.
        if (it->y0_round == y && current_x <= last_x) {
            // If y0_round = the current y, then delete any previous intersecting lines (to not count a 'line' twice)
            //     otherwise, given lines (p0, p1) and (p1, p2), where y0 < y1 < y2, 'p1' will be counted twice
            // Edge case: second polygon with (pa, p1) (p1, pb) (pb, pc) where y1 == yb [trapezoid]
            //     will delete (p0, p1) and (pa, p1) — but not (pb, pc).
            //     aka two edges have been deleted; # of total edges is still odd; final w = 0 not happening.

            // Store the edge and delete from its current position
            PathEdge edge = *it;
            it = edges.erase(it);
            // Find the new index to insert at
            it--;
            int newPosition = i - 1;
            for (; newPosition > 0; newPosition--) {
                it--;
                if (current_x > it->row_x) {
                    it++;
                    break;
                } else if (current_x == it->row_x && it->y1_round == y) {
                    // If the lines intersect and the iterator-line ends @ the current y, delete the iterator-line.
                    it = edges.erase(it);
                    i--;
                    k--;
                }
            }
            // Insert at the new index
            edges.insert(it, edge);
            newPosition++;
            // Move iterator back to the original position
            for (; newPosition < i; newPosition++) {
                it++;
            }
        } else if (current_x < last_x) {
            // Store the edge and delete from its current position
            PathEdge edge = *it;
            it = edges.erase(it);
            // Find the new index to insert at
            it--;
            int newPosition = i - 1;
            for (; newPosition > 0; newPosition--) {
                it--;
                if (current_x >= it->row_x) {
                    it++;
                    break;
                }
            }
            // Delete the edge from its current position and insert at the new index
            edges.insert(it, edge);
            newPosition++;
            // Move iterator back to the original position
            for (; newPosition < i; newPosition++) {
                it++;
            }
        }

        last_x = it->row_x;

        it++;
    }
    return k;
}

/**
 * Clamps and creates PathEdge(s) from given (p0, p1). Appends to provided std::list<PathEdge>.
 */
int appendPathEdge(float x0, float y0, float x1, float y1, int direction, int canvasBottom, int canvasRight, std::list<PathEdge> &edges, int bottom_pixel) {
    PathEdge newEdge;
    // Clipping - Vertical
    if ((y0 < 0 && y1 < 0) || (y0 > canvasBottom && y1 > canvasBottom)) {
        return bottom_pixel;
    }

    if (y0 < 0) {
        x0 = findNewA1(x0, x1, y0, y1, 0);
        y0 = 0;
    }

    if (y1 > canvasBottom) {
        bottom_pixel = canvasBottom;
        x1 = findNewA1(x1, x0, y1, y0, canvasBottom);
        y1 = canvasBottom;
    }

    // Clipping - Horizontal
    if (x0 < 0 && x1 < 0) {
        newEdge = makePathEdge(0.f, y0, 0.f, y1, direction, canvasRight);
        if (newEdge.y0_round != newEdge.y1_round) {
            edges.push_back(newEdge);
            if (newEdge.y1_round > bottom_pixel) {
                bottom_pixel = newEdge.y1_round;
            }
        }
        return bottom_pixel;
    } else if (x0 > canvasRight && x1 > canvasRight) {
        newEdge = makePathEdge(canvasRight, y0, canvasRight, y1, direction, canvasRight);
        if (newEdge.y0_round != newEdge.y1_round) {
            edges.push_back(newEdge);
            if (newEdge.y1_round > bottom_pixel) {
                bottom_pixel = newEdge.y1_round;
            }
        }
        return bottom_pixel;
    }

    if (x0 < 0) {
        float prevy0 = y0;
        y0 = std::ceil(findNewA1(y0, y1, x0, x1, 0));
        x0 = findNewA1(x0, x1, prevy0, y1, y0);
        newEdge = makePathEdge(0.f, prevy0, 0.f, y0, direction, canvasRight);
        if (newEdge.y0_round != newEdge.y1_round) {
            edges.push_back(newEdge);
        }
    } else if (x1 < 0) {
        float prevy1 = y1;
        y1 = std::floor(findNewA1(y1, y0, x1, x0, 0));
        x1 = findNewA1(x1, x0, prevy1, y0, y1);
        newEdge = makePathEdge(0.f, y1, 0.f, prevy1, direction, canvasRight);
        if (newEdge.y0_round != newEdge.y1_round) {
            edges.push_back(newEdge);
            if (newEdge.y1_round > bottom_pixel) {
                bottom_pixel = newEdge.y1_round;
            }
        }
    }

    if (x0 > canvasRight) {
        float prevy0 = y0;
        y0 = std::ceil(findNewA1(y0, y1, x0, x1, canvasRight));
        x0 = findNewA1(x0, x1, prevy0, y1, y0);
        newEdge = makePathEdge(canvasRight, prevy0, canvasRight, y0, direction, canvasRight);
        if (newEdge.y0_round != newEdge.y1_round) {
            edges.push_back(newEdge);
        }
    } else if (x1 > canvasRight) {
        float prevy1 = y1;
        y1 = std::floor(findNewA1(y1, y0, x1, x0, canvasRight));
        x1 = findNewA1(x1, x0, prevy1, y0, y1);
        newEdge = makePathEdge(canvasRight, y1, canvasRight, prevy1, direction, canvasRight);
        if (newEdge.y0_round != newEdge.y1_round) {
          edges.push_back(newEdge);
          if (newEdge.y1_round > bottom_pixel) {
              bottom_pixel = newEdge.y1_round;
          }
        }
    }

    // Make Edge
    newEdge = makePathEdge(x0, y0, x1, y1, direction, canvasRight);
    if (newEdge.y0_round != newEdge.y1_round) {
        edges.push_back(newEdge);
        if (newEdge.y1_round > bottom_pixel) {
            bottom_pixel = newEdge.y1_round;
        }
    }

    return bottom_pixel;
}



// ASSIGNMENT FUNCTIONS
void MyCanvas::clear(const GColor& color) {
    // GColor -> GPixel
    GPixel newPixel = GColorToGPixel(color);

    int height = fDevice.height();
    int width = fDevice.width();
    for (int y = 0; y < height; y++) {
        GPixel* pixels = fDevice.getAddr(0, y);
        for (int x = 0; x < width; x++) {
			*pixels = newPixel;
            pixels++;
        }
    }
}

void MyCanvas::drawRect(const GRect& rect, const GPaint& paint) {
    // Determine Shader vs Color and blendMode. Set Shader Context if applicable.
    GShader* shader = paint.peekShader();
    GColor color = paint.getColor();
    bool useShader = paint.peekShader();
    GBlendMode blendMode = paint.getBlendMode();

    if (useShader) {
        shader->setContext(currentMatrix);
        blendMode = simplifyBlendMode(blendMode, shader->isOpaque());
    } else {
        blendMode = simplifyBlendMode(blendMode, color.a);
    }

    // If rectangle makes no impact, terminate early.
    if (blendMode == GBlendMode::kDst) {
        return;
    }

    // If no transform is being performed, use rectangle sides; else, use transformed points.
    int left_border, right_border, top_border, bottom_border;
    if (currentMatrix == GMatrix()) {
        left_border = GRoundToInt(rect.left);
        right_border = GRoundToInt(rect.right);
        top_border = GRoundToInt(rect.top);
        bottom_border = GRoundToInt(rect.bottom);
    } else {
        // Transform points
        GPoint points[4];
        points[0] = makePoint(rect.left, rect.top);
        points[1] = makePoint(rect.right, rect.top);
        points[2] = makePoint(rect.right, rect.bottom);
        points[3] = makePoint(rect.left, rect.bottom);
        GPoint transformPoints[3];
        currentMatrix.mapPoints(transformPoints, points, 3);

        // If transform causes shape to no longer be a rectangle (or rotates 90/270), call drawPolygon().
        if (transformPoints[0].y != transformPoints[1].y || transformPoints[1].x != transformPoints[2].x) {
            drawConvexPolygon(points, 4, paint);
            return;
        }

        left_border = GRoundToInt(std::min(transformPoints[0].x, transformPoints[1].x));
        right_border = GRoundToInt(std::max(transformPoints[0].x, transformPoints[1].x));
        top_border = GRoundToInt(std::min(transformPoints[0].y, transformPoints[2].y));
        bottom_border = GRoundToInt(std::max(transformPoints[0].y, transformPoints[2].y));
    }

    // Check if rect is out-of-bounds or has a width/height of 0
    if (left_border >= fDevice.width()
        || right_border <= 0
        || top_border >= fDevice.height()
        || bottom_border <= 0
        || left_border == right_border
        || top_border == bottom_border) {
        return;
    }

    // Isolate drawn rectangle to the overlapping area between rect and fDevice
    left_border = std::max(left_border, 0);
    right_border = std::min(right_border, fDevice.width());
    top_border = std::max(top_border, 0);
    bottom_border = std::min(bottom_border, fDevice.height());

    // GShader vs GColor; draw rectangle.
    if (useShader && blendMode != GBlendMode::kClear) {
        int width = right_border - left_border;
        // if GShader replaces existing pixels, deploy a faster for-loop.
        if (blendMode == GBlendMode::kSrc) {
            for (int y = top_border; y < bottom_border; y++) {
                GPixel* pixelRow = fDevice.getAddr(left_border, y);
                GPixel shaderRow[width];
                shader->shadeRow(left_border, y, width, shaderRow);
                for (int i = 0; i < width; i++) {
                    pixelRow[i] = shaderRow[i];
                }
            }
        } else {
            BlendFunc* blendFunc = getBlendFunc(blendMode);

            for (int y = top_border; y < bottom_border; y++) {
                GPixel* pixelRow = fDevice.getAddr(left_border, y);
                GPixel shaderRow[width];
                shader->shadeRow(left_border, y, width, shaderRow);
                for (int i = 0; i < width; i++) {
                    pixelRow[i] = blendFunc(
                        pixelRow[i],
                        GPixel_GetA(shaderRow[i]),
                        GPixel_GetR(shaderRow[i]),
                        GPixel_GetG(shaderRow[i]),
                        GPixel_GetB(shaderRow[i])
                    );
                }
            }
        }
    } else {
        // If rectangle replaces existing pixels, deploy a faster for-loop.
        if (blendMode == GBlendMode::kClear || blendMode == GBlendMode::kSrc) {
            GPixel newPixel = (blendMode == GBlendMode::kClear) ? GPixel_PackARGB(0, 0, 0, 0) : GColorToGPixel(color);
            for (int y = top_border; y < bottom_border; y++) {
        	    GPixel* pixelRow = fDevice.getAddr(left_border, y);
        	    for (int x = left_border; x < right_border; x++) {
            	    *pixelRow = newPixel;
            	    pixelRow++;
        	    }
    	    }
        } else {
            // GColor -> GPixel rgbint
            int a = GRoundToInt(color.a * 255);
            int r = GRoundToInt(color.r * 255 * color.a);
            int g = GRoundToInt(color.g * 255 * color.a);
            int b = GRoundToInt(color.b * 255 * color.a);

            BlendFunc* blendFunc = getBlendFunc(blendMode);

    	    for (int y = top_border; y < bottom_border; y++) {
        	    GPixel* pixelRow = fDevice.getAddr(left_border, y);
        	    for (int x = left_border; x < right_border; x++) {
            	    *pixelRow = blendFunc(*pixelRow, a, r, g, b);
            	    pixelRow++;
        	    }
    	    }
        }
    }
}

void MyCanvas::drawConvexPolygon(const GPoint points[], int count, const GPaint& paint) {
    // Terminate if invalid polygon
    if (count < 3) {
        return;
    }

    // Determine Shader vs Color and blendMode. Set Shader Context if applicable.
    GShader* shader = paint.peekShader();
    GColor color = paint.getColor();
    bool useShader = paint.peekShader();
    GBlendMode blendMode = paint.getBlendMode();

    if (useShader) {
        shader->setContext(currentMatrix);
        blendMode = simplifyBlendMode(blendMode, shader->isOpaque());
    } else {
        blendMode = simplifyBlendMode(blendMode, color.a);
    }

    // Terminate if kDst.
    if (blendMode == GBlendMode::kDst) {
        return;
    }


    // PA3: Transform points according to currentMatrix.
    GPoint newPoints[count];
    currentMatrix.mapPoints(newPoints, points, count);

    // Create list of edges, determine bottom_pixel
    int canvasBottom = fDevice.height() - 1;
    int canvasRight = fDevice.width() - 1;

    float x0, y0, x1, y1;

    float prev_x = newPoints[count - 1].x;
    float prev_y = newPoints[count - 1].y;

    std::list<Edge> edges;
    Edge newEdge;
    int bottom_pixel = 0;

    for (int i = -1; i < count - 1; i++) {
        // Init
        if (prev_y < newPoints[i+1].y) {
        	x0 = prev_x;
        	y0 = prev_y;
        	x1 = newPoints[i + 1].x;
        	y1 = newPoints[i + 1].y;

            prev_x = x1;
            prev_y = y1;
        } else {
        	x0 = newPoints[i + 1].x;
        	y0 = newPoints[i + 1].y;
        	x1 = prev_x;
        	y1 = prev_y;

            prev_x = x0;
            prev_y = y0;
        }

        // Clipping - Vertical
        if ((y0 < 0 && y1 < 0) || (y0 > canvasBottom && y1 > canvasBottom)) {
            continue;
        }

        if (y0 < 0) {
            x0 = findNewA1(x0, x1, y0, y1, 0);
            y0 = 0;
        }

        if (y1 > canvasBottom) {
            bottom_pixel = canvasBottom;
            x1 = findNewA1(x1, x0, y1, y0, canvasBottom);
            y1 = canvasBottom;
        }

        // Clipping - Horizontal
        if (x0 < 0 && x1 < 0) {
            newEdge = makeEdge(0.f, y0, 0.f, y1);
            if (newEdge.y0_round != newEdge.y1_round) {
            	edges.push_back(newEdge);
                if (newEdge.y1_round > bottom_pixel) {
                    bottom_pixel = newEdge.y1_round;
                }
            }
            continue;
        } else if (x0 > canvasRight && x1 > canvasRight) {
            newEdge = makeEdge(canvasRight, y0, canvasRight, y1);
            if (newEdge.y0_round != newEdge.y1_round) {
            	edges.push_back(newEdge);
                if (newEdge.y1_round > bottom_pixel) {
                    bottom_pixel = newEdge.y1_round;
                }
            }
            continue;
        }

        if (x0 < 0) {
            float prevy0 = y0;
            y0 = findNewA1(y0, y1, x0, x1, 0);
            x0 = 0;
            newEdge = makeEdge(0.f, prevy0, 0.f, y0);
            if (newEdge.y0_round != newEdge.y1_round) {
            	edges.push_back(newEdge);
            }
        } else if (x1 < 0) {
            float prevy1 = y1;
            y1 = findNewA1(y1, y0, x1, x0, 0);
            x1 = 0;
            newEdge = makeEdge(0.f, y1, 0.f, prevy1);
            if (newEdge.y0_round != newEdge.y1_round) {
            	edges.push_back(newEdge);
                if (newEdge.y1_round > bottom_pixel) {
                    bottom_pixel = newEdge.y1_round;
                }
            }
        }

        if (x0 > canvasRight) {
            float prevy0 = y0;
            y0 = findNewA1(y0, y1, x0, x1, canvasRight);
            x0 = canvasRight;
            newEdge = makeEdge(canvasRight, prevy0, canvasRight, y0);
            if (newEdge.y0_round != newEdge.y1_round) {
            	edges.push_back(newEdge);
            }
        } else if (x1 > canvasRight) {
            float prevy1 = y1;
            y1 = findNewA1(y1, y0, x1, x0, canvasRight);
            x1 = canvasRight;
            newEdge = makeEdge(canvasRight, y1, canvasRight, prevy1);
            if (newEdge.y0_round != newEdge.y1_round) {
            	edges.push_back(newEdge);
                if (newEdge.y1_round > bottom_pixel) {
                    bottom_pixel = newEdge.y1_round;
                }
            }
        }

        // Make Edge
        newEdge = makeEdge(x0, y0, x1, y1);
        if (newEdge.y0_round != newEdge.y1_round) {
            edges.push_back(newEdge);
            if (newEdge.y1_round > bottom_pixel) {
                bottom_pixel = newEdge.y1_round;
            }
        }
    }

    if (edges.size() < 2) {
        return;
    }


    // Sort edges
    edges.sort();


    // Init edge1, edge2 and ensure L/R status; determine top_pixel
    std::list<Edge>::iterator it = edges.begin();
    Edge edge1 = *it;
    it++;
    Edge edge2 = *it;
    int top_pixel = std::max(edge1.y0_round, edge2.y0_round);


    // GShader vs GColor; draw polygon.
    if (useShader && blendMode != GBlendMode::kClear) {
        // if GShader replaces existing pixels, deploy a faster for-loop.
        if (blendMode == GBlendMode::kSrc) {
            for (int y = top_pixel; y < bottom_pixel; y++) {
                // Update leftEdge, rightEdge
                if (edge1.y1_round < y) {
                    it++;
                    edge1 = *it;
                }
                if (edge2.y1_round < y) {
                    it++;
                    edge2 = *it;
                }
                // Determine left_pixel, right_pixel; obtain shaderRow.
                int pixel_x1 = edge1.findX(y);
                int pixel_x2 = edge2.findX(y);
                int left_pixel = std::min(pixel_x1, pixel_x2);
                int right_pixel = std::max(pixel_x1, pixel_x2);
                int width = right_pixel - left_pixel;
                if (width == 0) {
                    continue;
                }
                GPixel* pixelRow = fDevice.getAddr(left_pixel, y);
                shader->shadeRow(left_pixel, y, width, pixelRow);
            }
        } else {
            BlendFunc* blendFunc = getBlendFunc(blendMode);

            for (int y = top_pixel; y < bottom_pixel; y++) {
                // Update leftEdge, rightEdge
                if (edge1.y1_round < y) {
                    it++;
                    edge1 = *it;
                }
                if (edge2.y1_round < y) {
                    it++;
                    edge2 = *it;
                }
                // Determine left_pixel, right_pixel; obtain shaderRow.
                int pixel_x1 = edge1.findX(y);
                int pixel_x2 = edge2.findX(y);
                int left_pixel = std::max(std::min(pixel_x1, pixel_x2), 0);
                int right_pixel = std::max(pixel_x1, pixel_x2);
                int width = right_pixel - left_pixel;
                if (width == 0) {
                    continue;
                }

                GPixel* pixelRow = fDevice.getAddr(left_pixel, y);
                GPixel shaderRow[width];
                shader->shadeRow(left_pixel, y, width, shaderRow);
                for (int i = 0; i < width; i++) {
                    pixelRow[i] = blendFunc(
                        pixelRow[i],
                        GPixel_GetA(shaderRow[i]),
                        GPixel_GetR(shaderRow[i]),
                        GPixel_GetG(shaderRow[i]),
                        GPixel_GetB(shaderRow[i])
                    );
                }
            }
        }
    } else {
        // If polygon replaces existing pixels, deploy a faster for-loop.
        if (blendMode == GBlendMode::kClear || blendMode == GBlendMode::kSrc) {
            GPixel newPixel = (blendMode == GBlendMode::kClear) ? GPixel_PackARGB(0, 0, 0, 0) : GColorToGPixel(color);

            for (int y = top_pixel; y < bottom_pixel; y++) {
                // Update leftEdge, rightEdge
                if (edge1.y1_round < y) {
                    it++;
                    edge1 = *it;
                }
                if (edge2.y1_round < y) {
                    it++;
                    edge2 = *it;
                }
                // Determine left_pixel, right_pixel
                int pixel_x1 = GRoundToInt(edge1.mx * y + edge1.bx);
                int pixel_x2 = GRoundToInt(edge2.mx * y + edge2.bx);
                int left_pixel = std::min(pixel_x1, pixel_x2);
                int right_pixel = std::max(pixel_x1, pixel_x2);

                // Find and loop through pixelRow
        	    GPixel* pixelRow = fDevice.getAddr(left_pixel, y);
                for (int x = left_pixel; x < right_pixel; x++) {
            	    *pixelRow = newPixel;
            	    pixelRow++;
        	    }
    	    }
        } else {
            // GColor -> GPixel rgbint
            int a = GRoundToInt(color.a * 255);
            int r = GRoundToInt(color.r * 255 * color.a);
            int g = GRoundToInt(color.g * 255 * color.a);
            int b = GRoundToInt(color.b * 255 * color.a);

            BlendFunc* blendFunc = getBlendFunc(blendMode);

    	    for (int y = top_pixel; y < bottom_pixel; y++) {
                // Update leftEdge, rightEdge
                if (edge1.y1_round < y) {
                    it++;
                    edge1 = *it;
                }
                if (edge2.y1_round < y) {
                    it++;
                    edge2 = *it;
                }
                // Determine left_pixel, right_pixel
                int pixel_x1 = GRoundToInt(edge1.mx * y + edge1.bx);
                int pixel_x2 = GRoundToInt(edge2.mx * y + edge2.bx);
                int left_pixel = std::min(pixel_x1, pixel_x2);
                int right_pixel = std::max(pixel_x1, pixel_x2);

                // Find and loop through pixelRow
        	    GPixel* pixelRow = fDevice.getAddr(left_pixel, y);
        	    for (int x = left_pixel; x < right_pixel; x++) {
            	    *pixelRow = blendFunc(*pixelRow, a, r, g, b);
            	    pixelRow++;
        	    }
    	    }
        }
    }
}

void MyCanvas::drawPath(const GPath& path, const GPaint& paint) {
    int count = path.countPoints();
    // Terminate if invalid polygon
    if (count < 3) {
        return;
    }


    // Determine Shader vs Color and blendMode. Set Shader Context if applicable.
    GShader* shader = paint.peekShader();
    GColor color = paint.getColor();
    bool useShader = paint.peekShader();
    GBlendMode blendMode = paint.getBlendMode();

    if (useShader) {
        shader->setContext(currentMatrix);
        blendMode = simplifyBlendMode(blendMode, shader->isOpaque());
    } else {
        blendMode = simplifyBlendMode(blendMode, color.a);
    }

    // Terminate if kDst.
    if (blendMode == GBlendMode::kDst) {
        return;
    }



    // Transform points according to currentMatrix.
    std::shared_ptr<GPath> transformPath = path.transform(currentMatrix);

    // Create list of edges, determine bottom_pixel
    int canvasBottom = fDevice.height() - 1;
    int canvasRight = fDevice.width() - 1;

    int bottom_pixel = 0;

    std::list<PathEdge> edges;
    GPoint pts[GPath::kMaxNextPoints];
    GPath::Edger edger(*transformPath);

    while (auto v = edger.next(pts)) {
        if (v.value() == GPathVerb::kLine) {
            // Init p0, p1 for the new edge
            if (pts[0].y < pts[1].y) {
            	bottom_pixel = appendPathEdge(pts[0].x, pts[0].y, pts[1].x, pts[1].y, 1, canvasBottom, canvasRight, edges, bottom_pixel);
            } else {
            	bottom_pixel = appendPathEdge(pts[1].x, pts[1].y, pts[0].x, pts[0].y, -1, canvasBottom, canvasRight, edges, bottom_pixel);
            }
        } else if (v.value() == GPathVerb::kQuad) {
            // Find n = number of line segments
            GPoint e0 = pts[0] - 2*pts[1] + pts[2];  // no /4 because /Tolerance = *4.
            float e0_dist = sqrt(pow(e0.x, 2.f) + pow(e0.y, 2.f));
            float n = ceilf(sqrt(e0_dist));

            // Ensure n is a valid number to loop over
            if (n > 1) {
                GPoint c_1 = 2*(pts[1] - pts[0]);

                float dt = 1.f / n;
                GPoint p0 = pts[0];
                for (float t = dt; t <= 1; t += dt) {
                    GPoint p1 = (e0*t + c_1)*t + pts[0];
                    if (p0.y < p1.y) {
                        bottom_pixel = appendPathEdge(p0.x, p0.y, p1.x, p1.y, 1, canvasBottom, canvasRight, edges, bottom_pixel);
                    } else {
                        bottom_pixel = appendPathEdge(p1.x, p1.y, p0.x, p0.y, -1, canvasBottom, canvasRight, edges, bottom_pixel);
                    }
                    p0 = p1;
                }
            } else {
                // Only drawing one line from A to C.
                if (pts[0].y < pts[2].y) {
                    bottom_pixel = appendPathEdge(pts[0].x, pts[0].y, pts[2].x, pts[2].y, 1, canvasBottom, canvasRight, edges, bottom_pixel);
                } else {
                    bottom_pixel = appendPathEdge(pts[2].x, pts[2].y, pts[0].x, pts[0].y, -1, canvasBottom, canvasRight, edges, bottom_pixel);
                }
            }
        } else if (v.value() == GPathVerb::kCubic) {
            GPoint c_3 = 3*pts[1] - pts[0] - 3*pts[2] + pts[3];
            GPoint e0 = c_3 * 0.5f;  // no /8 because /Tolerance = *4; 3 / 2 = 1.5
            float e0_dist = sqrt(pow(e0.x, 2.f) + pow(e0.y, 2.f));
            float n = ceilf(sqrt(e0_dist));

			// Ensure n is a valid number to loop over
            if (n > 1) {
                GPoint c_1 = 3*(pts[1] - pts[0]);
                GPoint c_2 = 3*(pts[0] - 2*pts[1] + pts[2]);

                float dt = 1.f / n;
                GPoint p0 = pts[0];
                for (float t = dt; t <= 1; t += dt) {
                    GPoint p1 = ((c_3*t + c_2)*t + c_1)*t + pts[0];
                    if (p0.y < p1.y) {
                        bottom_pixel = appendPathEdge(p0.x, p0.y, p1.x, p1.y, 1, canvasBottom, canvasRight, edges, bottom_pixel);
                    } else {
                        bottom_pixel = appendPathEdge(p1.x, p1.y, p0.x, p0.y, -1, canvasBottom, canvasRight, edges, bottom_pixel);
                    }
                    p0 = p1;
                }
            } else {
                // Only drawing one line from A to D.
                if (pts[0].y < pts[3].y) {
                    bottom_pixel = appendPathEdge(pts[0].x, pts[0].y, pts[3].x, pts[3].y, 1, canvasBottom, canvasRight, edges, bottom_pixel);
                } else {
                    bottom_pixel = appendPathEdge(pts[3].x, pts[3].y, pts[0].x, pts[0].y, -1, canvasBottom, canvasRight, edges, bottom_pixel);
                }
            }
        }
    }

    if (edges.size() < 2) {
        return;
    }



    // Sort edges
    edges.sort();


    // Determine top_pixel, k
    int top_pixel = edges.front().y0_round;
    int k = 0;
    for (auto it = edges.begin(); it != edges.end(); it++) {
      if (it->y0_round > top_pixel) {
        break;
      }
      k++;
    }


    // GShader vs GColor; draw polygon.
    if (useShader && blendMode != GBlendMode::kClear) {
        // if GShader replaces existing pixels, deploy a faster for-loop.
        if (blendMode == GBlendMode::kSrc) {
            for (int y = top_pixel; y < bottom_pixel; y++) {
                std::list<PathEdge>::iterator it;
                if (k > 0) {
                    // Sort the first k edges by x
                    k = sortFirstK(edges, k, y);
                    // Loop through edges
                    it = edges.begin();
                    int w = 0;
                    int left_pixel, width;
                    int iMax = k;
                    for (int i = 0; i < iMax; i++) {
                        int edge_x = it->row_x;
                        // If left pixel, instantiate
                        if (w == 0) {
                            left_pixel = edge_x;
                        }
                        w += it->direction;
                        // If row segment is over and width > 0, fill.
                        if (w == 0 && (width = edge_x - left_pixel) > 0) {
                            // Obtain pixelRow, shaderRow.
                            GPixel* pixelRow = fDevice.getAddr(left_pixel, y);
                            shader->shadeRow(left_pixel, y, width, pixelRow);
                        }
                        // If edge is valid, it++; else, remove
                        if (it->isValid(y+1)) {
                            it++;
                        } else {
                            it = edges.erase(it);
                            k--;
                        }
                    }
                } else {
                    it = edges.begin();
                }

                // Loop through the remaining elements and increment k as necessary
                while (it != edges.end()) {
                    if (it->isValid(y+1)) {
                        k++;
                        it++;
                    } else {
                        break;
                    }
                }
            }
        } else {
            BlendFunc* blendFunc = getBlendFunc(blendMode);

            for (int y = top_pixel; y < bottom_pixel; y++) {
                std::list<PathEdge>::iterator it;
                if (k > 0) {
                    // Sort the first k edges by x
                    k = sortFirstK(edges, k, y);
                    // Loop through edges
                    it = edges.begin();
                    int w = 0;
                    int left_pixel, width;
                    int iMax = k;
                    for (int i = 0; i < iMax; i++) {
                        int edge_x = it->row_x;
                        // If left pixel, instantiate
                        if (w == 0) {
                            left_pixel = edge_x;
                        }
                        w += it->direction;
                        // If row segment is over and width > 0, fill.
                        if (w == 0 && (width = edge_x - left_pixel) > 0) {
                            // Obtain pixelRow, shaderRow.
                            GPixel* pixelRow = fDevice.getAddr(left_pixel, y);
                            GPixel shaderRow[width];
                            shader->shadeRow(left_pixel, y, width, shaderRow);
                            // Fill in the row segment
                            for (int j = 0; j < width; j++) {
                                pixelRow[j] = blendFunc(
                                    pixelRow[j],
                                    GPixel_GetA(shaderRow[j]),
                                    GPixel_GetR(shaderRow[j]),
                                    GPixel_GetG(shaderRow[j]),
                                    GPixel_GetB(shaderRow[j])
                                );
                            }
                        }
                        // If edge is valid, it++; else, remove
                        if (it->isValid(y+1)) {
                            it++;
                        } else {
                            it = edges.erase(it);
                            k--;
                        }
                    }
                } else {
                    it = edges.begin();
                }

                // Loop through the remaining elements and increment k as necessary
                while (it != edges.end()) {
                    if (it->isValid(y+1)) {
                        k++;
                        it++;
                    } else {
                        break;
                    }
                }
            }
        }
    } else {
        // If polygon replaces existing pixels, deploy a faster for-loop.
        if (blendMode == GBlendMode::kClear || blendMode == GBlendMode::kSrc) {
            GPixel newPixel = (blendMode == GBlendMode::kClear) ? GPixel_PackARGB(0, 0, 0, 0) : GColorToGPixel(color);

            for (int y = top_pixel; y < bottom_pixel; y++) {
                std::list<PathEdge>::iterator it;
                // Perform operations if edge exists
                if (k > 0) {
                    // Sort the first k edges by x
                    k = sortFirstK(edges, k, y);
                    it = edges.begin();
                    int w = 0;
                    int left_pixel, width;
                    int iMax = k;
                    for (int i = 0; i < iMax; i++) {
                        int edge_x = it->row_x;
                        // If left pixel, instantiate
                        if (w == 0) {
                            left_pixel = edge_x;
                        }
                        w += it->direction;
                        // If row segment is over and width > 0, fill.
                        if (w == 0 && (width = edge_x - left_pixel) > 0) {
                            // Obtain pixelRow, shaderRow.
                            GPixel* pixelRow = fDevice.getAddr(left_pixel, y);
                            for (int j = 0; j < width; j++) {
                                pixelRow[j] = newPixel;
                            }
                        }
                        // If edge is valid, it++; else, remove
                        if (it->isValid(y+1)) {
                            it++;
                        } else {
                            it = edges.erase(it);
                            k--;
                        }
                    }
                } else {
                    it = edges.begin();
                }

                // Loop through the remaining elements and increment k as necessary
                while (it != edges.end()) {
                    if (it->isValid(y+1)) {
                        k++;
                        it++;
                    } else {
                        break;
                    }
                }
            }
        } else {
            // GColor -> GPixel rgbint
            int a = GRoundToInt(color.a * 255);
            int r = GRoundToInt(color.r * 255 * color.a);
            int g = GRoundToInt(color.g * 255 * color.a);
            int b = GRoundToInt(color.b * 255 * color.a);

            BlendFunc* blendFunc = getBlendFunc(blendMode);

            for (int y = top_pixel; y < bottom_pixel; y++) {
                std::list<PathEdge>::iterator it;
                if (k > 0) {
                    // Sort the first k edges by x
                    k = sortFirstK(edges, k, y);
                    // Loop through edges
                    it = edges.begin();
                    int w = 0;
                    int left_pixel, width;
                    int iMax = k;
                    for (int i = 0; i < iMax; i++) {
                        int edge_x = it->row_x;
                        // If left pixel, instantiate
                        if (w == 0) {
                            left_pixel = edge_x;
                        }
                        w += it->direction;
                        // If row segment is over and width > 0, fill.
                        if (w == 0 && (width = edge_x - left_pixel) > 0) {
                            // Obtain pixelRow, shaderRow.
                            GPixel* pixelRow = fDevice.getAddr(left_pixel, y);
                            for (int j = 0; j < width; j++) {
                                pixelRow[j] = blendFunc(pixelRow[j], a, r, g, b);
                            }
                        }
                        // If edge is valid, it++; else, remove
                        if (it->isValid(y+1)) {
                            it++;
                        } else {
                            it = edges.erase(it);
                            k--;
                        }
                    }
                } else {
                    it = edges.begin();
                }

                // Loop through the remaining elements and increment k as necessary
                while (it != edges.end()) {
                    if (it->isValid(y+1)) {
                        k++;
                        it++;
                    } else {
                        break;
                    }
                }
            }
        }
    }
}

void MyCanvas::drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[],
                              int count, const int indices[], const GPaint& paint) {
    int n = 0;

    if (texs == NULL) { // Color
        for (int i = 0; i < count; i++) {
            int ind0 = indices[n];
            int ind1 = indices[n + 1];
            int ind2 = indices[n + 2];
            std::shared_ptr<GShader> shader = GCreateTriGradientShader(
                verts[ind0], verts[ind1], verts[ind2],
                colors[ind0], colors[ind1], colors[ind2]
            );
            GPaint texPaint = GPaint(shader);
    		const GPoint triVerts[] = {verts[ind0], verts[ind1], verts[ind2]};
            drawConvexPolygon(triVerts, 3, texPaint);

            n += 3;
        }
    } else if (colors == NULL) { // Texture
        GShader* bmShader = paint.peekShader();
        for (int i = 0; i < count; i++) {
            int ind0 = indices[n];
            int ind1 = indices[n + 1];
            int ind2 = indices[n + 2];
            std::shared_ptr<GShader> shader = GCreateTriStickingShader(
                verts[ind0], verts[ind1], verts[ind2],
                texs[ind0], texs[ind1], texs[ind2],
                bmShader
            );
            GPaint stickPaint = GPaint(shader);
    		const GPoint triVerts[] = {verts[ind0], verts[ind1], verts[ind2]};
            drawConvexPolygon(triVerts, 3, stickPaint);

            n += 3;
        }
    } else { // Color && Texture
        GShader* bmShader = paint.peekShader();
        for (int i = 0; i < count; i++) {
            int ind0 = indices[n];
            int ind1 = indices[n + 1];
            int ind2 = indices[n + 2];
            std::shared_ptr<GShader> gradShader = GCreateTriGradientShader(
                verts[ind0], verts[ind1], verts[ind2],
                colors[ind0], colors[ind1], colors[ind2]
            );
            std::shared_ptr<GShader> stickShader = GCreateTriStickingShader(
                verts[ind0], verts[ind1], verts[ind2],
                texs[ind0], texs[ind1], texs[ind2],
                bmShader
            );
            std::shared_ptr<GShader> shader = GCreateTriComposeShader(gradShader.get(), stickShader.get());
            GPaint composePaint = GPaint(shader);
    		const GPoint triVerts[] = {verts[ind0], verts[ind1], verts[ind2]};
            drawConvexPolygon(triVerts, 3, composePaint);

            n += 3;
        }
    }
}

void MyCanvas::drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4],
                              int level, const GPaint& paint) {
    // Note: Assumes that level >= 0
    if (level == 0) {
        int finalIndices[6];
        // Triangle 1: abd
        finalIndices[0] = 0;
        finalIndices[1] = 1;
        finalIndices[2] = 3;
        // Triangle 2: bdc
        finalIndices[3] = 1;
        finalIndices[4] = 3;
        finalIndices[5] = 2;

        drawMesh(verts, colors, texs, 6, finalIndices, paint);
        return;
    }

    int numLines = level + 2;
    int numVerts = numLines * numLines;
    int numTriangles = (level + 1) * (level + 1) * 2;
    float sideFraction = 1 / (float) (level + 1);
    GPoint ad_inc = (verts[3] - verts[0]) * sideFraction;
    GPoint bc_inc = (verts[2] - verts[1]) * sideFraction;
    GPoint left = verts[0];
    GPoint right = verts[1];
    int rowStartInd = 0;
    int indicesInd = 0;

    GPoint finalVerts[numVerts];
    int finalIndices[numTriangles * 3];

    if (texs == NULL) { // Color
        GColor finalColors[numVerts];
        GColor ad_cInc = (colors[3] - colors[0]) * sideFraction;
        GColor bc_cInc = (colors[2] - colors[1]) * sideFraction;
        GColor leftC = colors[0];
        GColor rightC = colors[1];

        // Create first row - prevents an extra row of triangles from being added to finalIndices
        finalVerts[0] = left;
        GPoint lr_inc = (right - left) * sideFraction;
        GPoint lowerRight = left;

        finalColors[0] = colors[0];
        GColor lr_cInc = (colors[1] - colors[0]) * sideFraction;
        GColor lowerRightC = colors[0];
        for (int j = 1; j < numLines; j++) {
            lowerRight += lr_inc;
            finalVerts[j] = lowerRight;
            lowerRightC += lr_cInc;
            finalColors[j] = lowerRightC;
        }

        // Main loop through remaining vertex rows
        for (int i = 1; i < numLines; i++) {
            int prevLrInd = rowStartInd;
            rowStartInd += numLines;
            left += ad_inc;
            right += bc_inc;
            leftC += ad_cInc;
            rightC += bc_cInc;

            finalVerts[rowStartInd] = left;
            lr_inc = (right - left) * sideFraction;
            lowerRight = left;

            finalColors[rowStartInd] = leftC;
            lr_cInc = (rightC - leftC) * sideFraction;
            lowerRightC = leftC;

            int lrInd = rowStartInd;
            for (int j = 1; j < numLines; j++) {
                // Triangle 1: abd
                finalIndices[indicesInd++] = prevLrInd++;
                finalIndices[indicesInd++] = prevLrInd;
                finalIndices[indicesInd++] = lrInd;

                // Triangle 2: bdc  (and lrInd++)
                finalIndices[indicesInd++] = prevLrInd;
                finalIndices[indicesInd++] = lrInd++;
                finalIndices[indicesInd++] = lrInd;

                // Add the new vertex + color
                lowerRight += lr_inc;
                finalVerts[lrInd] = lowerRight;
            	lowerRightC += lr_cInc;
            	finalColors[lrInd] = lowerRightC;
            }
        }

        drawMesh(finalVerts, finalColors, NULL, numTriangles, finalIndices, paint);
    } else if (colors == NULL) { // Texture
        GPoint finalTextures[numVerts];
        GPoint ad_tInc = (texs[3] - texs[0]) * sideFraction;
        GPoint bc_tInc = (texs[2] - texs[1]) * sideFraction;
        GPoint leftT = texs[0];
        GPoint rightT = texs[1];

        // Create first row - prevents an extra row of triangles from being added to finalIndices
        finalVerts[0] = left;
        GPoint lr_inc = (right - left) * sideFraction;
        GPoint lowerRight = left;

        finalTextures[0] = texs[0];
        GPoint lr_tInc = (texs[1] - texs[0]) * sideFraction;
        GPoint lowerRightT = texs[0];
        for (int j = 1; j < numLines; j++) {
            lowerRight += lr_inc;
            finalVerts[j] = lowerRight;
            lowerRightT += lr_tInc;
            finalTextures[j] = lowerRightT;
        }

        // Main loop through remaining vertex rows
        for (int i = 1; i < numLines; i++) {
            int prevLrInd = rowStartInd;
            rowStartInd += numLines;
            left += ad_inc;
            right += bc_inc;
            leftT += ad_tInc;
            rightT += bc_tInc;

            finalVerts[rowStartInd] = left;
            lr_inc = (right - left) * sideFraction;
            lowerRight = left;

            finalTextures[rowStartInd] = leftT;
            lr_tInc = (rightT - leftT) * sideFraction;
            lowerRightT = leftT;

            int lrInd = rowStartInd;
            for (int j = 1; j < numLines; j++) {
                // Triangle 1: abd
                finalIndices[indicesInd++] = prevLrInd++;
                finalIndices[indicesInd++] = prevLrInd;
                finalIndices[indicesInd++] = lrInd;

                // Triangle 2: bdc  (and lrInd++)
                finalIndices[indicesInd++] = prevLrInd;
                finalIndices[indicesInd++] = lrInd++;
                finalIndices[indicesInd++] = lrInd;

                // Add the new vertex + texture
                lowerRight += lr_inc;
                finalVerts[lrInd] = lowerRight;
            	lowerRightT += lr_tInc;
            	finalTextures[lrInd] = lowerRightT;
            }
        }

        drawMesh(finalVerts, NULL, finalTextures, numTriangles, finalIndices, paint);
    } else { // Color && Texture
        GColor finalColors[numVerts];
        GColor ad_cInc = (colors[3] - colors[0]) * sideFraction;
        GColor bc_cInc = (colors[2] - colors[1]) * sideFraction;
        GColor leftC = colors[0];
        GColor rightC = colors[1];

        GPoint finalTextures[numVerts];
        GPoint ad_tInc = (texs[3] - texs[0]) * sideFraction;
        GPoint bc_tInc = (texs[2] - texs[1]) * sideFraction;
        GPoint leftT = texs[0];
        GPoint rightT = texs[1];

        // Create first row - prevents an extra row of triangles from being added to finalIndices
        finalVerts[0] = left;
        GPoint lr_inc = (right - left) * sideFraction;
        GPoint lowerRight = left;

        finalColors[0] = colors[0];
        GColor lr_cInc = (colors[1] - colors[0]) * sideFraction;
        GColor lowerRightC = colors[0];

        finalTextures[0] = texs[0];
        GPoint lr_tInc = (texs[1] - texs[0]) * sideFraction;
        GPoint lowerRightT = texs[0];
        for (int j = 1; j < numLines; j++) {
            lowerRight += lr_inc;
            finalVerts[j] = lowerRight;
            lowerRightC += lr_cInc;
            finalColors[j] = lowerRightC;
            lowerRightT += lr_tInc;
            finalTextures[j] = lowerRightT;
        }

        // Main loop through remaining vertex rows
        for (int i = 1; i < numLines; i++) {
            int prevLrInd = rowStartInd;
            rowStartInd += numLines;
            left += ad_inc;
            right += bc_inc;
            leftC += ad_cInc;
            rightC += bc_cInc;
            leftT += ad_tInc;
            rightT += bc_tInc;

            finalVerts[rowStartInd] = left;
            lr_inc = (right - left) * sideFraction;
            lowerRight = left;

            finalColors[rowStartInd] = leftC;
            lr_cInc = (rightC - leftC) * sideFraction;
            lowerRightC = leftC;

            finalTextures[rowStartInd] = leftT;
            lr_tInc = (rightT - leftT) * sideFraction;
            lowerRightT = leftT;

            int lrInd = rowStartInd;
            for (int j = 1; j < numLines; j++) {
                // Triangle 1: abd
                finalIndices[indicesInd++] = prevLrInd++;
                finalIndices[indicesInd++] = prevLrInd;
                finalIndices[indicesInd++] = lrInd;

                // Triangle 2: bdc  (and lrInd++)
                finalIndices[indicesInd++] = prevLrInd;
                finalIndices[indicesInd++] = lrInd++;
                finalIndices[indicesInd++] = lrInd;

                // Add the new vertex, color, and texture
                lowerRight += lr_inc;
                finalVerts[lrInd] = lowerRight;
            	lowerRightC += lr_cInc;
            	finalColors[lrInd] = lowerRightC;
            	lowerRightT += lr_tInc;
            	finalTextures[lrInd] = lowerRightT;
            }
        }

        drawMesh(finalVerts, finalColors, finalTextures, numTriangles, finalIndices, paint);
    }

}



// MATRIX FUNCTIONS
void MyCanvas::save() {
    // Creates a copy of the current matrix by multiplying it with the identity matrix.
    GMatrix newMatrix = GMatrix(currentMatrix * GMatrix());
    savedMatrices.push_back(newMatrix);
}
void MyCanvas::restore() {
    // Assumes that savedMatrices is not empty, as per definition.
    currentMatrix = savedMatrices.back();
    savedMatrices.pop_back();
}
void MyCanvas::concat(const GMatrix& newMatrix) {
    currentMatrix = currentMatrix * newMatrix;
}



std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& device) {
    return std::unique_ptr<GCanvas>(new MyCanvas(device));
}

std::string GDrawSomething(GCanvas* canvas, GISize dim) {
    int width = dim.width;
    int height = dim.height;

    // Add a background.
    GBitmap background = GBitmap();
    background.readFromFile("my_background.png");
    float scaleX = (float) width / background.width();
    float scaleY = (float) height / background.height();
    GMatrix fillCanvas = GMatrix::Scale(scaleX, scaleY);
    std::shared_ptr<GShader> bgShader = GCreateBitmapShader(background, fillCanvas);
    canvas->drawRect(GRect::LTRB(0, 0, width, height), GPaint(bgShader));

    float edge = 0;
    float x1 = 0.25f * width;
    float x2 = 0.5f * width;
    float x3 = 0.75f * width;
    float x4 = width;
    float y1 = 0.25f * height;
    float y2 = 0.5f * height;
    float y3 = 0.75f * height;
    float y4 = height;


    // Turn it up
    canvas->rotate(0.1f);
    canvas->scale(1.f, 1.1f);

    // Add a white background to the rotated section
    canvas->fillRect(GRect::LTRB(edge, edge, x4, y4), GColor::RGBA(0.f, 0.f, 0.f, 1.f));

    // Main 4 squares(?)
    canvas->fillRect(GRect::LTRB(edge, edge, x2, y2), GColor::RGBA(0.2f, 0.2f, 0.2f, 0.5f));
    canvas->fillRect(GRect::LTRB(x2, edge, x4, y2), GColor::RGBA(0.4f, 0.4f, 0.2f, 0.5f));
    canvas->fillRect(GRect::LTRB(edge, y2, x2, y4), GColor::RGBA(0.6f, 0.3f, 0.6f, 0.5f));
    canvas->fillRect(GRect::LTRB(x2, y2, x4, y4), GColor::RGBA(0.4f, 0.8f, 0.8f, 0.5f));

    // Center square(?)
    canvas->fillRect(GRect::LTRB(x1, y1, x3, y3), GColor::RGBA(1.f, 0.1f, 0.1f, 0.5f));

	// Corner squares(?)
    canvas->fillRect(GRect::LTRB(edge, edge, x1, y1), GColor::RGBA(0.215f, 0.698f, 0.004f, 0.5f));
    canvas->fillRect(GRect::LTRB(x3, edge, x4, y1), GColor::RGBA(0.f, 0.714f, 1.f, 0.5f));
    canvas->fillRect(GRect::LTRB(edge, y3, x1, y4), GColor::RGBA(0.933f, 0.502f, 0.47f, 0.5f));
    canvas->fillRect(GRect::LTRB(x3, y3, x4, y4), GColor::RGBA(0.757f, 0.137f, 0.925f, 0.5f));

    // Diamonds(?)
    GPoint diamond1[] = {{edge, y1}, {x1, edge}, {x3, edge}, {x4, y1}, {x2, y3}};
    canvas->drawConvexPolygon(diamond1, 4, GPaint(GColor::RGBA(0.941f, 0.494f, 0.725f, 0.6f)));
    GPoint diamond2[] = {{edge, y3}, {x1, y4}, {x3, y4}, {x4, y3}, {x2, y1}};
    canvas->drawConvexPolygon(diamond2, 4, GPaint(GColor::RGBA(0.737f, 0.341f, 0.125f, 0.6f)));

    return "peril-elograms";
}

