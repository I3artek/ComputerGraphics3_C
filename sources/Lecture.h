//
// Created by I3artek on 20/04/2025.
//

#ifndef MY_RAYLIB_GAME_LECTURE_H
#define MY_RAYLIB_GAME_LECTURE_H

#include "Canvas.h"

// code snippets from lecture slides

void MidpointLine(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int d = 2 * dy - dx;
    int dE = 2 * dy;
    int dNE = 2 * (dy - dx);
    int x = x1, y = y1;
    put_pixel(x, y, 255, 0, 0);
    while (x < x2)
    {
        if ( d < 0 ) // move to E
        {
            d += dE;
            x++;
        }
        else // move to NE
        {
            d += dNE;
            ++x;
            ++y;
        }
        put_pixel(x, y, 255, 0, 0);
    }
}

#endif //MY_RAYLIB_GAME_LECTURE_H
