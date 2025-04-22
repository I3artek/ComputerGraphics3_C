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

struct point {
    int x;
    int y;
};
typedef struct point point;

extern Texture2D canvas_texture;

class Shape {
public:
    // draw the shape to the canvas
    virtual void draw() = 0;
    // add next point to shape
    virtual shape_state add_point(int x, int y) = 0;
    // returns a point or nullptr if not a vertex
    virtual point *get_vertex(int x, int y) = 0;
    virtual shape_state get_state() = 0;
    virtual shape_state set_state(shape_state s) = 0;
    virtual void move_shape(int dx, int dy) = 0;
};

class Line: public Shape {
private:
    shape_state state = NOT_FINISHED;
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

    void set_color(uint8_t r, uint8_t g, uint8_t b)
    {
        return;
    }

    void new_vertices(int x0, int y0, int x1, int y1)
    {
        points[0].x = x0;
        points[0].y = y0;
        points[1].x = x1;
        points[1].y = y1;
    }

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
        if(count == 2) {
            state = FINISHED;
        }
        return get_state();
    }

    shape_state get_state()
    {
        if(count != 2) {
            state = NOT_FINISHED;
        }
        return state;
    }
    shape_state set_state(shape_state s)
    {
        state = s;
    }

    point *get_vertex(int x, int y) {
        printf("Checking for vertex near [%d, %d]\n", x, y);
        // pixel-perfect
        for(int i = 0; i < count; i++) {
            if(points[i].x == x && points[i].y == y) {
                printf("Returning vertex #%d\n", i);
                return &points[i];
            }
        }
        // in proximity
        int x_max, x_min, y_max, y_min;
        int radius = 10;
        for(int i = 0; i < count; i++) {
            x_max = points[i].x + radius;
            x_min = points[i].x - radius;
            y_max = points[i].y + radius;
            y_min = points[i].y - radius;
            if(x < x_max && x > x_min && y < y_max && y > y_min) {
                printf("Returning vertex #%d\n", i);
                return &points[i];
            }
        }
        return nullptr;
    }

    void move_shape(int dx, int dy)
    {
        for(int i = 0; i < count; i++)
        {
            points[i].x += dx;
            points[i].y += dy;
        }
    }
};

class Polygon: public Shape {
private:
    shape_state state = NOT_FINISHED;
    // number of points is also the number of lines
    const static int max_points = 20;
    point points[max_points];
    int count = 0;
    int width = 1;
    Line lines[max_points];
public:
    void set_width(int w)
    {
        if(w % 2 == 0) {
            return;
        }
        width = w;
    }

    void set_color(uint8_t r, uint8_t g, uint8_t b)
    {
        return;
    }

    void regenerate_lines()
    {
        if(get_state() != FINISHED) {
            return;
        }
        point *p0;
        point *p1;
        for(int i = 0; i < count - 1; i++)
        {
            p0 = &points[i];
            p1 = &points[(i + 1) % count];
            lines[i].new_vertices(p0->x, p0->y, p1->x, p1->y);
        }
    }

    void draw()
    {
        regenerate_lines();
        for(int i = 0; i < count; i++)
        {
            lines[i].draw();
        }
    }

    point *get_vertex(int x, int y) {
        printf("Checking for vertex near [%d, %d]\n", x, y);
        // pixel-perfect
        for(int i = 0; i < count; i++) {
            if(points[i].x == x && points[i].y == y) {
                printf("Returning vertex #%d\n", i);
                return &points[i];
            }
        }
        // in proximity
        int x_max, x_min, y_max, y_min;
        int radius = 10;
        for(int i = 0; i < count; i++) {
            x_max = points[i].x + radius;
            x_min = points[i].x - radius;
            y_max = points[i].y + radius;
            y_min = points[i].y - radius;
            if(x < x_max && x > x_min && y < y_max && y > y_min) {
                printf("Returning vertex #%d\n", i);
                return &points[i];
            }
        }
        return nullptr;
    }

    shape_state add_point(int x, int y)
    {
        // if clicked near the start point, finish the shape
        int radius = 10;
        int x_max = points[0].x + radius;
        int x_min = points[0].x - radius;
        int y_max = points[0].y + radius;
        int y_min = points[0].y - radius;
        if(x < x_max && x > x_min && y < y_max && y > y_min) {
            if(count < 3) {
                printf("Cannot finish - need at least 3 vertices!\n");
                return NOT_FINISHED;
            }
            printf("Finishing polygon\n");
            state = FINISHED;
        }
        points[count++] = {
                .x = x,
                .y = y
        };
        // mark the pixel's spot with some color
        // it will be overriden anyway when drawing the shape
        put_pixel(x, y, 0, 0, 255);
        UpdateTexture(canvas_texture, canvas.data);
        if(count == max_points - 1) {
            state = FINISHED;
        }
        return get_state();
    }

    shape_state get_state()
    {
        if(count < 3) {
            state = NOT_FINISHED;
        }
        return state;
    }
    shape_state set_state(shape_state s)
    {
        state = s;
    }

    void move_shape(int dx, int dy)
    {
        for(int i = 0; i < count; i++)
        {
            points[i].x += dx;
            points[i].y += dy;
        }
    }
};


#endif //MY_RAYLIB_GAME_SHAPE_H
