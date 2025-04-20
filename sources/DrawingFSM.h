//
// Created by I3artek on 17/04/2025.
//

#ifndef MY_RAYLIB_GAME_DRAWINGFSM_H
#define MY_RAYLIB_GAME_DRAWINGFSM_H

#include "Shape.h"

extern Texture2D canvas_texture;

class DrawingFSM {
public:
    Shape *current_shape;
    // I don't have a clue why I am unable to import proper header to be able to use std::vector,
    // and honestly I don't care, this is not something that should be graded on this subject
    Shape *shapes[20];
    int idx = 0;
    //point points[CANVAS_SIZE][CANVAS_SIZE];
    //int p_idx = 0;

    void handle_click(Vector2 pos)
    {
        if(current_shape == nullptr) {
            return;
        }
        if(pos.x < CANVAS_SIZE && pos.y < CANVAS_SIZE) {
            printf("x: %f, y: %f\n", pos.x, pos.y);
            shape_state ret;
            ret = current_shape->add_point((int) pos.x, (int) pos.y);
            if(ret == FINISHED) {
                current_shape->draw();
                UpdateTexture(canvas_texture, canvas.data);
                current_shape = nullptr;
            }
        }
    }

    void draw_line()
    {
        current_shape = new Line();
        shapes[idx++] = current_shape;
    }
};


#endif //MY_RAYLIB_GAME_DRAWINGFSM_H
