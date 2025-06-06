//
// Created by I3artek on 17/04/2025.
//

#ifndef MY_RAYLIB_GAME_DRAWINGFSM_H
#define MY_RAYLIB_GAME_DRAWINGFSM_H

#include "Shape.h"
#include <iostream>
#include <fstream>

extern Texture2D canvas_texture;

enum state {
    DRAW = 0,
    EDIT,
    MOVE_PIXEL,
    IDLE,
    DELETE,
    MOVE_SHAPE,
    RESIZE_POLY,
    ROTATE_POLY,
    CLIP_POLY
};
const char* states[] = {
        "DRAWING - SELECT NEW VERTEX",
        "EDITING - SELECT VERTEX TO MOVE",
        "MOVING - SELECT NEW VERTEX POSITION",
        "IDLE - CHOOSE ACTION BELOW",
        "DELETING - SELECT A SHAPE TO DELETE",
        "MOVING - SELECT AND MOVE A SHAPE",
        "Resizing polygon",
        "Rotating polygon",
        "Clipping polygon"
};

class DrawingFSM {
public:
    bool AA = false;
    Shape *current_shape;
    // I don't have a clue why I am unable to import proper header to be able to use std::vector,
    // and honestly I don't care, this is not something that should be graded on this subject
    Shape *shapes[20];
    int count = 0;
    //point points[CANVAS_SIZE][CANVAS_SIZE];
    //int p_idx = 0;
    enum state state = IDLE;
    point *p = nullptr;
    Color current_color = {255, 70, 30, 255};
    int current_width = 1;
    char width_text[18] = "Current width: 01";
    bool resizing_circle = false;
    point chosen_point = { .x = 0, .y = 0};
    bool has_point = false;
    float scale = 0.5;
    int angle = 0;

    void save_to_file()
    {
        std::ofstream VectorData("save.txt");
        // we save only FINISHED shapes
        // need to count them first
        int new_count = 0;
        for(int i = 0; i < count; i++)
        {
            if(shapes[i]->get_state() == FINISHED) {
                new_count++;
            }
        }
        // write count of shapes to file
        VectorData << new_count << " \n";
        VectorData << scale << "\n";
        VectorData << angle << "\n";
        // write all shapes data
        for(int i = 0; i < count; i++)
        {
            if(shapes[i]->get_state() != FINISHED) {
                continue;
            }
            Line *line;
            Polygon *polygon;
            Circle *circle;
            if(dynamic_cast<Line *>(shapes[i]) != nullptr) {
                // line
                line = dynamic_cast<Line *>(shapes[i]);
                VectorData << "L ";
                VectorData << line << "\n";
            } else if(dynamic_cast<Polygon *>(shapes[i]) != nullptr) {
                polygon = dynamic_cast<Polygon *>(shapes[i]);
                VectorData << "P ";
                VectorData << polygon << "\n";
            } else if(dynamic_cast<Circle *>(shapes[i]) != nullptr) {
                circle = dynamic_cast<Circle *>(shapes[i]);
                VectorData << "C ";
                VectorData << circle << "\n";
            }
        }
        VectorData.close();
    }

    void load_from_file()
    {
        std::ifstream VectorData("save.txt");
        VectorData >> count;
        VectorData >> scale;
        VectorData >> angle;
        char type;
        for(int i = 0; i < count; i++)
        {
            Line *line;
            Polygon *polygon;
            Circle *circle;
            VectorData >> type;
            if(type == 'L') {
                printf("Loading line\n");
                line = new Line();
                VectorData >> line;
                shapes[i] = line;
            } else if(type == 'P') {
                printf("Loading polygon\n");
                polygon = new Polygon();
                VectorData >> polygon;
                shapes[i] = polygon;
            } else if(type == 'C') {
                printf("Loading circle\n");
                circle = new Circle();
                VectorData >> circle;
                shapes[i] = circle;
            }
        }
        VectorData.close();
        redraw_all();
    }

    void redraw_all()
    {
        // clear the canvas
        pixel *pi;
        for(int i = 0; i < CANVAS_SIZE; i++)
        {
            for(int j = 0; j < CANVAS_SIZE; j++)
            {
                pi = get_pixel(canvas, i, j);
                pi->r = 0xff;
                pi->g = 0xff;
                pi->b = 0xff;
                pi->a = 0xff;
            }
        }
        for(int i = 0; i < count; i++) {
            if(shapes[i]->get_state() != FINISHED) {
                continue;
            }
            shapes[i]->draw();
        }
        UpdateTexture(canvas_texture, canvas.data);
    }

    void handle_click(Vector2 pos)
    {
        int x = (int)pos.x;
        int y = (int)pos.y;
        printf("x: %d, y: %d\n", x, y);
        if(x >= CANVAS_SIZE || y >= CANVAS_SIZE || x < 0 || y < 0) {
            return;
        }
        if(state == EDIT) {
            for(int i = 0; i < count; i++) {
                if(shapes[i]->get_state() != FINISHED) {
                    //printf("Shapes: %d, i: %d!\n", count, i);
                    continue;
                }
                p = shapes[i]->get_vertex(x, y);
                if(p != nullptr) {
                    state = MOVE_PIXEL;
                    if(dynamic_cast<Circle *>(shapes[i]) != nullptr) {
                        resizing_circle = true;
                    }
                    break;
                }
            }
        } else if(state == MOVE_PIXEL) {
            if(p == nullptr) {
                printf("Something went wrong!\n");
                state = IDLE;
                resizing_circle = false;
                return;
            }
            if(resizing_circle) {
                resizing_circle = false;
            }
            p->x = x;
            p->y = y;
            p = nullptr;
            redraw_all();
            state = IDLE;
        } else if(state == DRAW) {
            if(current_shape == nullptr) {
                return;
            }
            shape_state ret;
            ret = current_shape->add_point(x, y);
            if(ret == FINISHED) {
                current_shape = nullptr;
                redraw_all();
                state = IDLE;
            }
        } else if(state == DELETE) {
            for(int i = 0; i < count; i++) {
                if(shapes[i]->get_state() != FINISHED) {
                    //printf("Shapes: %d, i: %d!\n", count, i);
                    continue;
                }
                p = shapes[i]->get_vertex(x, y);
                if(p != nullptr) {
                    shapes[i]->set_state(NOT_FINISHED);
                    redraw_all();
                    state = IDLE;
                    return;
                }
            }
        } else if(state == MOVE_SHAPE) {
            if(current_shape == nullptr) {
                // select the shape
                for(int i = 0; i < count; i++) {
                    if(shapes[i]->get_state() != FINISHED) {
                        //printf("Shapes: %d, i: %d!\n", count, i);
                        continue;
                    }
                    p = shapes[i]->get_vertex(x, y);
                    if(p != nullptr) {
                        current_shape = shapes[i];
                        break;
                    }
                }
            } else {
                // select new location for chosen vertex
                // all vertices will be moved by the same vector
                int dx = x - p->x;
                int dy = y - p->y;
                current_shape->move_shape(dx, dy);
                redraw_all();
                state = IDLE;
            }
        } else if(state == RESIZE_POLY) {
            if(current_shape == nullptr) {
                // select the shape
                for(int i = 0; i < count; i++) {
                    if(shapes[i]->get_state() != FINISHED) {
                        //printf("Shapes: %d, i: %d!\n", count, i);
                        continue;
                    }
                    p = shapes[i]->get_vertex(x, y);
                    if(p != nullptr) {
                        current_shape = shapes[i];
                        break;
                    }
                }
            } else {
                // select point
                chosen_point.x = x;
                chosen_point.y = y;
                Polygon *polygon;
                if(dynamic_cast<Polygon *>(current_shape) != nullptr) {
                    polygon = dynamic_cast<Polygon *>(current_shape);
                    polygon->resize(scale, &chosen_point);
                }
                redraw_all();
                has_point = true;
                state = IDLE;
                has_point = false;
                current_shape = nullptr;
            }
        } else if(state == ROTATE_POLY) {
            if(current_shape == nullptr) {
                // select the shape
                for(int i = 0; i < count; i++) {
                    if(shapes[i]->get_state() != FINISHED) {
                        //printf("Shapes: %d, i: %d!\n", count, i);
                        continue;
                    }
                    p = shapes[i]->get_vertex(x, y);
                    if(p != nullptr) {
                        current_shape = shapes[i];
                        break;
                    }
                }
            } else {
                // select point
                chosen_point.x = x;
                chosen_point.y = y;
                Polygon *polygon;
                if(dynamic_cast<Polygon *>(current_shape) != nullptr) {
                    polygon = dynamic_cast<Polygon *>(current_shape);
                    polygon->rotate(angle, &chosen_point);
                }
                redraw_all();
                has_point = true;
                state = IDLE;
                has_point = false;
                current_shape = nullptr;
            }
        } else if(state == CLIP_POLY) {
            if(current_shape == nullptr) {
                // select the shape
                for(int i = 0; i < count; i++) {
                    if(shapes[i]->get_state() != FINISHED) {
                        continue;
                    }
                    p = shapes[i]->get_vertex(x, y);
                    if(p != nullptr) {
                        current_shape = shapes[i];
                        break;
                    }
                }
            } else {
                Shape *clip_shape = nullptr;
                for(int i = 0; i < count; i++) {
                    if(shapes[i]->get_state() != FINISHED) {
                        continue;
                    }
                    p = shapes[i]->get_vertex(x, y);
                    if(p != nullptr) {
                        clip_shape = shapes[i];
                        break;
                    }
                }
                Polygon *poly;
                Polygon *clip_poly;
                if(dynamic_cast<Polygon *>(current_shape) != nullptr &&
                        dynamic_cast<Polygon *>(clip_shape) != nullptr) {
                    poly = dynamic_cast<Polygon *>(current_shape);
                    clip_poly = dynamic_cast<Polygon *>(clip_shape);
                    clip(poly, clip_poly);
                    redraw_all();
                }
                state = IDLE;
                current_shape = nullptr;
            }
        }
    }

    void draw_line() {
        if (state == DRAW) {
            return;
        }
        current_shape = new Line();
        shapes[count++] = current_shape;
        current_shape->set_color(current_color.r, current_color.g, current_color.b);
        current_shape->set_width(current_width);
        //printf("count: %d\n", count);
        state = DRAW;
    }

    void draw_polygon() {
        if (state == DRAW) {
            return;
        }
        current_shape = new Polygon();
        shapes[count++] = current_shape;
        current_shape->set_color(current_color.r, current_color.g, current_color.b);
        current_shape->set_width(current_width);
        //printf("count: %d\n", count);
        state = DRAW;
    }

    void draw_circle() {
        if (state == DRAW) {
            return;
        }
        current_shape = new Circle();
        shapes[count++] = current_shape;
        current_shape->set_color(current_color.r, current_color.g, current_color.b);
        current_shape->set_width(current_width);
        //printf("count: %d\n", count);
        state = DRAW;
    }


    void move_shape()
    {
        current_shape = nullptr;
        state = MOVE_SHAPE;
    }

    void edit_shape()
    {
        state = EDIT;
    }

    void delete_point()
    {
        state = DELETE;
    }

    void resize_poly()
    {
        state = RESIZE_POLY;
    }

    void rotate_poly()
    {
        state = ROTATE_POLY;
    }

    void clear()
    {
        // not cleaning garbage - I am fully aware of that
        count = 0;
        redraw_all();
    }

    void toggle_aa()
    {
        AA = !AA;
        for(int i = 0; i < count; i++)
        {
            shapes[i]->set_antialiasing(AA);
        }
        redraw_all();
    }

    void aa_on()
    {
        AA = true;
        for(int i = 0; i < count; i++)
        {
            shapes[i]->set_antialiasing(AA);
        }
        redraw_all();
    }
    void aa_off()
    {
        AA = false;
        for(int i = 0; i < count; i++)
        {
            shapes[i]->set_antialiasing(AA);
        }
        redraw_all();
    }

    void clip_poly()
    {
        state = CLIP_POLY;
    }

    void change_width(bool more)
    {
        const int max_width = 9;
        if(more && current_width < max_width) {
            current_width += 2;
            snprintf(width_text, 18, "Current width: %d", current_width);
        } else if(!more && current_width > 1) {
            current_width -= 2;
            snprintf(width_text, 18, "Current width: %d", current_width);
        }
    }

};


#endif //MY_RAYLIB_GAME_DRAWINGFSM_H
