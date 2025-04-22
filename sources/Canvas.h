//
// Created by I3artek on 17/04/2025.
//

#ifndef MY_RAYLIB_GAME_CANVAS_H
#define MY_RAYLIB_GAME_CANVAS_H

#include "raylib.h"
#include <stdint.h>

#define CANVAS_SIZE (200)

#define get_pixel(c, x, y) (&(((pixel *)(c.data))[x + CANVAS_SIZE * y]))
#define gp(x, y) get_pixel(canvas, (x), (y))

// drawing pixels is memory-safe
// any out-of bounds pixels are skipped
#define put_pixel(x, y, r_, g_, b_)                                \
do {                                                               \
    if(x > 0 && x < CANVAS_SIZE && y > 0 && y < CANVAS_SIZE) {     \
        pixel *p;                                                  \
        p = gp(x, y);                                              \
        p->r = r_;                                                 \
        p->g = g_;                                                 \
        p->b = b_;                                                 \
    }                                                              \
} while(0)


typedef union pixel_u {
    uint32_t data;
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
} pixel;

extern Image canvas;

#endif //MY_RAYLIB_GAME_CANVAS_H
