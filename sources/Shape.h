//
// Created by I3artek on 17/04/2025.
//

#ifndef MY_RAYLIB_GAME_SHAPE_H
#define MY_RAYLIB_GAME_SHAPE_H

#define swap(a, b) \
do {               \
int tmp = a;       \
a = b;             \
b = tmp;           \
} while(0)


typedef enum {
    FINISHED = 0,
    NOT_FINISHED,
} shape_state;

#include "Canvas.h"
#include "Lecture.h"

typedef struct {
    int x;
    int y;
} point;

class Shape {
public:
    // draw the shape to the canvas
    virtual void draw() = 0;
    // add
    virtual shape_state add_point(int x, int y) = 0;
    shape_state state;
};

class Line: public Shape {
private:
    point points[2];
    int count = 0;
    int width = 1;
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    void draw_horizontal(int x0, int y0, int x1, int y1) {
        int dx = x1 - x0;
        int dy = y1 - y0;
        int yi = 1;
        if(dy < 0) {
            yi = -1;
            dy = -dy;
        }
        int D = 2 * dy - dx;
        int y = y0;
        for(int x = x0; x <= x1; x++) {
            int r = (width - 1) / 2;
            while (r-- > 0) {
                put_pixel(x, y + r, 255, 0, 0);
                put_pixel(x, y - r, 255, 0, 0);
            }
            put_pixel(x, y, 255, 0, 0);
            if(D > 0) {
                y = y + yi;
                D = D + (2 * (dy - dx));
            } else {
                D = D + 2 * dy;
            }
        }
    }
    void draw_vertical(int x0, int y0, int x1, int y1) {
        int dx = x1 - x0;
        int dy = y1 - y0;
        int xi = 1;
        if(dx < 0) {
            xi = -1;
            dx = -dx;
        }
        int D = 2 * dx - dy;
        int x = x0;
        for(int y = y0; y <= y1; y++) {
            int r = (width - 1) / 2;
            while (r-- > 0) {
                put_pixel(x + r, y, 255, 0, 0);
                put_pixel(x - r, y, 255, 0, 0);
            }
            put_pixel(x, y, 255, 0, 0);
            if(D > 0) {
                x = x + xi;
                D = D + (2 * (dx - dy));
            } else {
                D = D + 2 * dx;
            }
        }
    }
public:
    void set_width(int w) {
        if(w % 2 == 0) {
            return;
        }
        width = w;
    }
    //shape_state state;
    void draw() {
        pixel *p;
        int x0 = points[0].x;
        int x1 = points[1].x;
        int y0 = points[0].y;
        int y1 = points[1].y;
        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        //printf("Drawing:\nx0:%d\nx1:%d\ny0:%d\ny1:%d\n", x0, x1, y0, y1);
        if(dy < dx) {
            if(x0 > x1) {
                draw_horizontal(x1, y1, x0, y0);
            } else {
                draw_horizontal(x0, y0, x1, y1);
            }
        } else {
            if(y0 > y1) {
                draw_vertical(x1, y1, x0, y0);
            } else {
                draw_vertical(x0, y0, x1, y1);
            }
        }
    }

    shape_state add_point(int x, int y)
    {
        points[count++] = {
                .x = x,
                .y = y
        };
        return count == 2 ? FINISHED : NOT_FINISHED;
    }
};


#endif //MY_RAYLIB_GAME_SHAPE_H
