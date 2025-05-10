//
// Created by I3artek on 24/04/2025.
//

#ifndef MY_RAYLIB_GAME_BEZIER_H
#define MY_RAYLIB_GAME_BEZIER_H

#include <unistd.h>
#include "raylib.h"
#include "Canvas.h"

#define MAX_POINTS 40

Vector2 Bezier_points[MAX_POINTS];

Vector2 operator + (Vector2 a, Vector2 b) {
    Vector2 c = {
            .x = a.x + b.x,
            .y = a.y + b.y
    };
    return c;
}

Vector2 operator - (Vector2 a, Vector2 b) {
    Vector2 c = {
            .x = a.x - b.x,
            .y = a.y - b.y
    };
    return c;
}

Vector2 operator * (float s, Vector2 a) {
    Vector2 c = {
            .x = a.x * s,
            .y = a.y * s
    };
    return c;
}

// https://stackoverflow.com/questions/785097/how-do-i-implement-a-bézier-curve-in-c
Vector2 getBezierPoint( Vector2* points, int numPoints, float t ) {
    Vector2* tmp = Bezier_points;
    memcpy(tmp, points, numPoints * sizeof(Vector2));
    int i = numPoints - 1;
    while (i > 0) {
        for (int k = 0; k < i; k++)
            tmp[k] = tmp[k] + t * ( tmp[k+1] - tmp[k] );
        i--;
    }
    Vector2 answer = tmp[0];
    return answer;
}

void draw_bezier_curve(Vector2* points, int numPoints) {
    Vector2 start = points[0];
    Vector2 end = points[numPoints - 1];
    Vector2 c = start;
    printf("Drawing Bezier curve with %d points\n", numPoints);
    for(float t = 0.0; t <= 1.0; t += 0.001) {
        c = getBezierPoint(points, numPoints, t);
        //printf("Got [%f, %f]\n", c.x, c.y);
        put_pixel((int)c.x, (int)c.y, 255, 0, 0);
    }
}

#endif //MY_RAYLIB_GAME_BEZIER_H
