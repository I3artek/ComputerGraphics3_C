//
// Created by I3artek on 17/04/2025.
//

#ifndef MY_RAYLIB_GAME_SHAPE_H
#define MY_RAYLIB_GAME_SHAPE_H

#include "Canvas.h"
#include "Lecture.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "math.h"
#include "AET.h"

#define USER_CLICK_ERROR 10

#define DBG() printf("%s %d\n", __func__, __LINE__)

#define swap(a, b) \
do {               \
int tmp = a;       \
a = b;             \
b = tmp;           \
} while(0)

// modf was producing SIGSEGV for unknown reasons
#define my_modf(a) (a - floorf(a))
#define get_color(L, B, c, s) (uint8_t)(L.c * (1 - my_modf(s)) + B.c * my_modf(s))

#define get_color_circle(L, B, c) (uint8_t)(L.c * (1 - T) + B.c * T)

typedef enum {
    FINISHED = 0,
    NOT_FINISHED,
} shape_state;

typedef enum {
    NO_POINT = 0,
    CENTER,
    PERIMETER
} circle_point;

struct point {
    int x;
    int y;
};
typedef struct point point;

std::ifstream& operator>>(std::ifstream& is, point *p)
{
    is >> p->x;
    is >> p->y;
    return is;
}

std::ofstream& operator<<(std::ofstream& os, const point *p)
{
    os << p->x << " ";
    os << p->y << " ";
    return os;
}

std::ifstream& operator>>(std::ifstream& is, pixel *p)
{
    // need a variable of type int for reading
    // otherwise this does not work
    int tmp;
    is >> tmp;
    p->r = tmp;
    is >> tmp;
    p->g = tmp;
    is >> tmp;
    p->b = tmp;
    return is;
}

std::ofstream& operator<<(std::ofstream& os, const pixel *p)
{
    os << (int)p->r << " ";
    os << (int)p->g << " ";
    os << (int)p->b << " ";
    return os;
}

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
    virtual void set_color(uint8_t r, uint8_t g, uint8_t b) = 0;
    virtual void set_width(int w) = 0;
    virtual void set_antialiasing(bool b) = 0;
    virtual bool get_antialiasing() = 0;
    //virtual void serialize
};

class Line: public Shape {
    friend std::ofstream& operator<<(std::ofstream& os, const Line *l);
    friend std::ifstream& operator>>(std::ifstream& is, Line *l);
private:
    bool aa = false;
    shape_state state = NOT_FINISHED;
    point points[2];
    int count = 0;
    int width = 1;
    pixel color = {
            .r = 255,
            .g = 0,
            .b = 0,
            .a = 255
    };
    void draw_horizontal_aa(int x0, int y0, int x1, int y1) {
        pixel L = color;
        pixel B = {
                .r = 255,
                .g = 255,
                .b = 255,
                .a = 255
        };
        auto y = (float)y0;
        float m = (float)(y1 - y0) / (float)(x1 - x0);
        double *tmp;
        for(int x = x0; x <= x1; x++)
        {
            //B = *gp((int)x,(int)y);
            pixel c1 = {
                    .r = get_color(L, B, r, y),
                    .g = get_color(L, B, g, y),
                    .b = get_color(L, B, b, y),
                    .a = 255
            };
            pixel c2 = {
                    .r = get_color(B, L, r, y),
                    .g = get_color(B, L, g, y),
                    .b = get_color(B, L, b, y),
                    .a = 255
            };
            put_pixel(x, (int)floorf(y), c1.r, c1.g, c1.b);
            put_pixel(x, (int)floorf(y) + 1, c2.r, c2.g, c2.b);
            y += m;
        }
    }
    void draw_vertical_aa(int x0, int y0, int x1, int y1) {
        pixel L = color;
        pixel B = {
                .r = 255,
                .g = 255,
                .b = 255,
                .a = 255
        };
        auto x = (float)x0;
        float m = (float)(x1 - x0) / (float)(y1 - y0);//dupa
        double *tmp;
        for(int y = y0; y <= y1; y++)
        {
            //B = *gp((int)x,(int)y);
            pixel c1 = {
                    .r = get_color(L, B, r, x),
                    .g = get_color(L, B, g, x),
                    .b = get_color(L, B, b, x),
                    .a = 255
            };
            pixel c2 = {
                    .r = get_color(B, L, r, x),
                    .g = get_color(B, L, g, x),
                    .b = get_color(B, L, b, x),
                    .a = 255
            };
            put_pixel((int)floorf(x), y, c1.r, c1.g, c1.b);
            put_pixel((int)floorf(x) + 1, y, c2.r, c2.g, c2.b);
            x += m;
        }
    }
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    void draw_horizontal(int x0, int y0, int x1, int y1) {
        if(aa) {
            draw_horizontal_aa(x0, y0, x1, y1);
            return;
        }
        // x0 < x1
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
            while (r > 0) {
                put_pixel(x, y + r, color.r, color.g, color.b);
                put_pixel(x, y - r, color.r, color.g, color.b);
                r--;
            }
            put_pixel(x, y, color.r, color.g, color.b);
            if(D > 0) {
                y = y + yi;
                D = D + (2 * (dy - dx));
            } else {
                D = D + 2 * dy;
            }
        }
    }
    void draw_vertical(int x0, int y0, int x1, int y1) {
        if(aa) {
            draw_vertical_aa(x0, y0, x1, y1);
            return;
        }
        // y0 < y1
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
            while (r > 0) {
                put_pixel(x + r, y, color.r, color.g, color.b);
                put_pixel(x - r, y, color.r, color.g, color.b);
                r--;
            }
            put_pixel(x, y, color.r, color.g, color.b);
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
        color.r = r;
        color.g = g;
        color.b = b;
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
        return state;
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
        int radius = USER_CLICK_ERROR;
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

    void set_antialiasing(bool b) {
        aa = b;
    }
    bool get_antialiasing() {
        return aa;
    }
};

std::ofstream& operator<<(std::ofstream& os, const Line *l)
{
    os << l->width << " ";
    os << &l->color << " ";
    os << l->count << " ";
    for(int i = 0; i < l->count; i++)
    {
        os << &l->points[i] << " ";
    }
    return os;
}
std::ifstream& operator>>(std::ifstream& is, Line *l)
{
    is >> l->width;
    is >> &l->color;
    is >> l->count;
    printf("Loaded count: %d\n", l->count);
    for(int i = 0; i < l->count; i++)
    {
        is >> &l->points[i];
    }
    l->set_state(FINISHED);
    return is;
}

edge *AET;
edge *ET[CANVAS_SIZE];

class Polygon: public Shape {
    friend std::ifstream& operator>>(std::ifstream& is, Polygon *l);
    friend std::ofstream& operator<<(std::ofstream& os, const Polygon *l);
private:
    bool aa = false;
    shape_state state = NOT_FINISHED;
    // number of points is also the number of lines
    const static int max_points = 40;
    point points[max_points];
    int count = 0;
    int width = 1;
    Line lines[max_points];
    pixel color = {
            .r = 255,
            .g = 0,
            .b = 0,
            .a = 255
    };
    std::vector<point> points_vector;
public:
    bool is_convex() {
        // TODO: fill this
        return true;
    }

    bool filled = true;

    // check if the vertices are in clockwise order
    bool is_clockwise() {
        point u = {
                points[1].x - points[0].x,
                points[1].y - points[0].y
        };
        point v = {
                points[2].x - points[1].x,
                points[2].y - points[1].y
        };
        return u.x * v.y - u.y * v.x > 0;
    }

    void remake(std::vector<point> new_points) {
        count = new_points.size();
        for(int i = 0; i < new_points.size(); i++)
        {
            points[i] = new_points[i];
        }
        regenerate_lines();
    }

    std::vector<point> get_points() {
        points_vector.clear();
        for(int i = 0; i < count; i++)
        {
            points_vector.push_back(points[i]);
        }
        return points_vector;
    }

    void resize(float scale, point *p)
    {
        int dx, dy;
        for(int i = 0; i < count; i++)
        {
            printf("dupa, x: %d, y: %d\n", points[i].x, points[i].y);
            dx = points[i].x - p->x;
            dy = points[i].y - p->y;
            printf("dupb, dx: %d, dy: %d\n", dx, dy);
            if(scale >= 1 || scale <= 0) {
                points[i].x += (scale - 1) * dx;
                points[i].y += (scale - 1) * dy;
            } else {
                points[i].x -= (scale) * dx;
                points[i].y -= (scale) * dy;
            }

            printf("dupc, x: %d, y: %d\n", points[i].x, points[i].y);
        }
    }

    void rotate(float angle, point *p)
    {
        double radians = angle * M_PI / 180;
        for(int i = 0; i < count; i++)
        {
            int x = points[i].x - p->x;
            int y = points[i].y - p->y;
            printf("dupa, x: %d, y: %d\n", points[i].x, points[i].y);
            points[i].x = (x * cos(radians) - y * sin(radians)) + p->x;
            points[i].y = (y * cos(radians) + x * sin(radians)) + p->y;
            printf("dupc, x: %d, y: %d\n", points[i].x, points[i].y);
        }
    }

    void set_width(int w)
    {
        if(w % 2 == 0) {
            return;
        }
        width = w;
    }

    void set_color(uint8_t r, uint8_t g, uint8_t b)
    {
        color.r = r;
        color.g = g;
        color.b = b;
    }

    void regenerate_lines()
    {
        if(get_state() != FINISHED) {
            return;
        }
        point *p0;
        point *p1;
        for(int i = 0; i < count; i++)
        {
            p0 = &points[i];
            p1 = &points[(i + 1) % count];
            lines[i].new_vertices(p0->x, p0->y, p1->x, p1->y);
            lines[i].set_color(color.r, color.g, color.b);
            lines[i].set_width(width);
            lines[i].set_antialiasing(aa);
        }
    }

    void draw()
    {
        regenerate_lines();
        for(int i = 0; i < count; i++)
        {
            lines[i].draw();
        }
        if(filled) {
            fill();
        }
    }

    void fill() {
        // clear edge tables
        std::vector<edge *> aet;
        for(int i = 0; i < CANVAS_SIZE; i++)
        {
            ET[i] = nullptr;
        }
        // fill the edge table
        point s = points[count - 1];
        point p = points[0];
        for(int j = 0; j < count; j++, p = points[j])
        {
            printf("[%s:%d] j: %d\n", __func__, __LINE__, j);
            int y_min = s.y < p.y ? s.y : p.y;
            auto e = new edge(s.x, s.y, p.x, p.y);
            printf("[%s:%d] y: %d\n", __func__, __LINE__, y_min);
            printf("[%s:%d] head: %p\n", __func__, __LINE__, ET[y_min]);
            insert(&ET[y_min], e);
            printf("[%s:%d] head: %p\n", __func__, __LINE__, ET[y_min]);
            s = p;
        }
        // actual color filling
        for(int y = 0; y < CANVAS_SIZE;)
        {
            insert_to_vector(&ET[y], aet);
            if(aet.size() < 1) {
                y++;
                continue;
            }
            printf("\n[%s:%d] y: %d - scanning\n", __func__, __LINE__, y);
            std::sort(aet.begin(), aet.end(),
                      [](edge *e1, edge *e2){ return e1->x_min < e2->x_min;});
            for(int i = 0; i < aet.size(); i++)
            {
                printf("X: %d, ", aet[i]->x_min);
            }
            printf("\n");
            // TODO: fill pixels between pairs
            bool filling = true;
            int next = 1;
            //printf("[%s:%d] aet size: %d\n", __func__, __LINE__, aet.size());
            //printf("[%s:%d] aet[0]: %p\n", __func__, __LINE__, aet[0]);
            for(int x = aet[0]->x_min; x < aet[aet.size() - 1]->x_min + 1; x++)
            {
                if(filling) {
                    //put_pixel(x, y, color.r, color.g, color.b);
                    put_pixel(x, y, color.r, color.g, color.b);

                }
                // if we arrive at an edge, change the filling status
                while(next < aet.size() && x == aet[next]->x_min) {
                    if(filling) {
                        put_pixel(x + 1, y, color.r, color.g, color.b);
                    }
                    filling = !filling;
                    next++;
                }
            }
            y++;
            // TODO: remove all edges from AET where y_max == y
            auto sth = std::remove_if(aet.begin(), aet.end(),
                           [y](edge *e) {
                               return e->y_max == y;
            });
            aet.erase(sth, aet.end());
            // TODO e.update_x() on all edges in AET
            std::for_each(aet.begin(), aet.end(), [](edge *e) {e->update_x();});
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
        int radius = USER_CLICK_ERROR;
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
        if(count > 0 && points[count - 1].x == x && points[count - 1].y == y)
        {
            // don't duplicate points in case of multiple clicks
            return get_state();
        }
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
            return get_state();
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
        return state;
    }

    void move_shape(int dx, int dy)
    {
        for(int i = 0; i < count; i++)
        {
            points[i].x += dx;
            points[i].y += dy;
        }
    }

    void set_antialiasing(bool b) {
        aa = b;
    }
    bool get_antialiasing() {
        return aa;
    }
};

std::ofstream& operator<<(std::ofstream& os, const Polygon *l)
{
    os << l->width << " ";
    os << &l->color << " ";
    os << l->count << " ";
    for(int i = 0; i < l->count; i++)
    {
        os << &l->points[i] << " ";
    }
    return os;
}
std::ifstream& operator>>(std::ifstream& is, Polygon *l)
{
    is >> l->width;
    is >> &l->color;
    is >> l->count;
    printf("Loaded count: %d\n", l->count);
    for(int i = 0; i < l->count; i++)
    {
        is >> &l->points[i];
    }
    l->set_state(FINISHED);
    return is;
}

class Circle: public Shape {
    friend std::ifstream &operator>>(std::ifstream &is, Circle *l);
    friend std::ofstream &operator<<(std::ofstream &os, const Circle *l);
private:
    bool aa = false;
    shape_state state = NOT_FINISHED;
    point center;
    int radius = -1;
    pixel color = {
            .r = 255,
            .g = 0,
            .b = 0,
            .a = 255
    };
    void draw_four_octants(int dx, int dy, pixel c = {}) {
        if(c.a == 0) {
            c = color;
        }
        put_pixel(center.x + dx, center.y + dy, c.r, c.g, c.b);
        put_pixel(center.x + dx, center.y - dy, c.r, c.g, c.b);
        put_pixel(center.x - dx, center.y + dy, c.r, c.g, c.b);
        put_pixel(center.x - dx, center.y - dy, c.r, c.g, c.b);
    }
public:
    // a variable to indicate whether the user clicked the center or a point on the perimeter
    circle_point last_clicked = NO_POINT;

    void set_width(int w)
    {
        return;
    }

    void draw_aa()
    {
        pixel L = color;
        pixel B = {
                .r = 255,
                .g = 255,
                .b = 255,
                .a = 255
        };
        int x = radius;
        int y = 0;
        //put_pixel(x, y, L.r, L.g, L.b);
        draw_four_octants(x, y);
        draw_four_octants(y, x);
        while(x > y)
        {
            y++;
            x = ceil(sqrt(radius * radius - y * y));
            float T = abs(x - sqrt(radius * radius - y * y));
            // D(R,y) = ceil(sqrt(radius * radius - y * y)) - sqrt(radius * radius - y * y)
            printf("%f\n", T);
            pixel c2 = {
                    .r = get_color_circle(L, B, r),
                    .g = get_color_circle(L, B, g),
                    .b = get_color_circle(L, B, b),
                    .a = 255
            };
            pixel c1 = {
                    .r = get_color_circle(B, L, r),
                    .g = get_color_circle(B, L, g),
                    .b = get_color_circle(B, L, b),
                    .a = 255
            };
            //put_pixel(center.x + x, center.y + y, c2.r, c2.g, c2.b);
            //put_pixel(center.x + x - 1, center.y + y, c1.r, c1.g, c1.b);
            draw_four_octants(x, y, c2);
            draw_four_octants(x - 1, y, c1);
            draw_four_octants(y, x, c2);
            draw_four_octants(y, x - 1, c1);
        }
    }

    void draw()
    {
        // draw the center for easier selection
        put_pixel(center.x, center.y, color.r, color.g, color.b);
        if(aa) {
            draw_aa();
            return;
        }
        int x = 0;
        int y = radius;
        int d = 1 - radius;
        draw_four_octants(x, y);
        draw_four_octants(y, x);
        while (y > x)
        {
            if(d < 0) {
                d += 2 * x + 3;
            } else {
                d += 2 * x - 2 * y + 5;
                y--;
            }
            x++;
            draw_four_octants(x, y);
            draw_four_octants(y, x);
        }

    }
    shape_state add_point(int x, int y)
    {
        if(radius == -1) {
            center.x = x;
            center.y = y;
            radius = 0;
            return get_state();
        } else {
            int dx = x - center.x;
            int dy = y - center.y;
            radius = sqrt(pow(dx, 2) + pow(dy, 2));
            state = FINISHED;
            return get_state();
        }
    }
    point *get_vertex(int x, int y)
    {
        // check if the center was clicked
        if(x == center.x && y == center.y) {
            printf("Returning center\n");
            last_clicked = CENTER;
            return &center;
        }
        int error = USER_CLICK_ERROR;
        int x_max = center.x + error;
        int x_min = center.x - error;
        int y_max = center.y + error;
        int y_min = center.y - error;
        if(x < x_max && x > x_min && y < y_max && y > y_min) {
            printf("Returning center\n");
            last_clicked = CENTER;
            return &center;
        }
        // you know what? screw it, the user cannot select a point on the perimeter

        last_clicked = NO_POINT;
        return nullptr;
    }
    shape_state get_state()
    {
        if(radius < 0) {
            state = NOT_FINISHED;
        }
        return state;
    }
    shape_state set_state(shape_state s)
    {
        state = s;
        return state;
    }
    void move_shape(int dx, int dy)
    {
        center.x += dx;
        center.y += dy;
    }
    void set_color(uint8_t r, uint8_t g, uint8_t b)
    {
        color.r = r;
        color.g = g;
        color.b = b;
    }

    void set_antialiasing(bool b) {
        aa = b;
    }
    bool get_antialiasing() {
        return aa;
    }
};

std::ofstream &operator<<(std::ofstream &os, const Circle *l) {
    os << &l->center << " ";
    os << &l->color << " ";
    os << l->radius << " ";
    return os;
}

std::ifstream &operator>>(std::ifstream &is, Circle *l) {
    is >> &l->center;
    is >> &l->color;
    is >> l->radius;
    l->set_state(FINISHED);
    return is;
}

bool is_inside(point p, point clipBoundary[2], bool clockwise = true) {
    // TODO: fill this
    // https://stackoverflow.com/questions/1560492/how-to-tell-whether-a-point-is-to-the-right-or-left-side-of-a-line
    point a = clockwise ? clipBoundary[0] : clipBoundary[1];
    point b = clockwise ? clipBoundary[1] : clipBoundary[0];
    return (b.x - a.x)*(p.y - a.y) - (b.y - a.y)*(p.x - a.x) > 0;
    return false;
}

// https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines
// intersect the intersection point may be stored in the floats i_x and i_y.
bool get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y,
                           float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        if (i_x != NULL)
            *i_x = p0_x + (t * s1_x);
        if (i_y != NULL)
            *i_y = p0_y + (t * s1_y);
        return 1;
    }

    return 0; // No collision
}

// get the intersection point of line a->b with poly
// this function assumes such a point exists
point intersect(point a, point b, point clipBoundary[2]) {
    // TODO: fill this
    float x = 0.0, y = 0.0;
    float ax = (float)a.x;
    float ay = (float)a.y;
    float bx = (float)b.x;
    float by = (float)b.y;
    point s = clipBoundary[0];
    point p = clipBoundary[1];
    get_line_intersection(ax, ay, bx, by,
                          (float) s.x, (float) s.y,
                          (float) p.x, (float) p.y,
                          &x, &y);
    return {(int)x, (int)y};
}

// poly - polygon to be clipped
// clip - clipping polygon
void clip(Polygon *poly, Polygon *clip_poly) {
    // we clip only to convex polygons
    if(!clip_poly->is_convex()) {
        return;
    }
    printf("Doing clipping!\n");
    // from the lecture slides
    std::vector<point> clipEdges = clip_poly->get_points();
    point clipBoundary[2] = {};
    std::vector<point> inPoly;
    std::vector<point> outPoly = poly->get_points();
    clipBoundary[0] = clipEdges[clipEdges.size() - 1];
    clipBoundary[1] = clipEdges[0];
    bool clockwise = clip_poly->is_clockwise();
    printf("clockwise: %d\n", clockwise);
    for(int k = 0; k < clipEdges.size(); k++, clipBoundary[1] = clipEdges[k])
    {
        inPoly = outPoly;
        outPoly.clear();
        point i, p = inPoly[0], s = inPoly[inPoly.size() - 1]; // start with the last point
        for(int j = 0; j < inPoly.size(); ++j, p = inPoly[j])
        {
            if(is_inside(p, clipBoundary, clockwise)) {
                if(is_inside(s, clipBoundary, clockwise)) {
                    printf("case 1.\n");
                    outPoly.push_back(p);
                    printf("adding %d, %d\n", p.x, p.y);
                } else {
                    printf("case 2.\n");
                    i = intersect(s, p, clipBoundary);
                    outPoly.push_back(i);
                    outPoly.push_back(p);
                    printf("adding %d, %d\n", i.x, i.y);
                    printf("adding %d, %d\n", p.x, p.y);
                }
            } else if(is_inside(s, clipBoundary, clockwise)) {
                printf("case 3.\n");
                i = intersect(s, p, clipBoundary);
                outPoly.push_back(i);
                printf("adding %d, %d\n", i.x, i.y);
            }
            s = p;
        }
        clipBoundary[0] = clipBoundary[1];
    }
    // TODO: here remake poly with new points
    poly->remake(outPoly);
    // TODO: and probably destroy the clipping polygon (?)
    clip_poly->set_state(NOT_FINISHED);
    return;
}


#endif //MY_RAYLIB_GAME_SHAPE_H
