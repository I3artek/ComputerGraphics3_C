#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <printf.h>
#include "Canvas.h"
#include "Shape.h"
#include "DrawingFSM.h"

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (450)

#define WINDOW_TITLE "Window title"

Image canvas = {
        .data = nullptr,
        .width = CANVAS_SIZE,
        .height = CANVAS_SIZE,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
};

Texture2D canvas_texture;

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    Texture2D texture = LoadTexture(ASSETS_PATH"test.png"); // Check README.md for how this works

    canvas.data = calloc(CANVAS_SIZE * CANVAS_SIZE,sizeof(uint32_t));
    if(canvas.data == nullptr)
    {
        return -1;
    }
    // fill the pixels
    printf("Fill the pixels - each %d\n", sizeof(pixel));
    pixel *p;
    for(int i = 0; i < CANVAS_SIZE; i++)
    {
        for(int j = 0; j < CANVAS_SIZE; j++)
        {
            //printf("Fill the pixel\n");
            p = get_pixel(canvas, i, j);
            p->r = 0xff;
            p->g = 0xff;
            p->b = 0xff;
            p->a = 0xff;
            //p->data = 0xffffffff;
        }
    }


    canvas_texture = LoadTextureFromImage(canvas);

    Line *l = new Line();
    DrawingFSM *fsm = new DrawingFSM();
    fsm->draw_line();

    while (!WindowShouldClose())
    {
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 pos = GetMousePosition();
            fsm->handle_click(pos);
            //printf("x: %f, y: %f\n", pos.x, pos.y);
//            if(pos.x < CANVAS_SIZE && pos.y < CANVAS_SIZE)
//            {
//                printf("x: %f, y: %f\n", pos.x, pos.y);
//
//                p = gp((int)pos.x, (int)pos.y);
//                p->r = 0;
//                p->g = 0;
//                p->b = 0;
//                p->a = 0;
//                UpdateTexture(canvas_texture, canvas.data);
//                l->draw();
//            }
        }

        if(IsKeyDown(KEY_L)) fsm->draw_line();

        BeginDrawing();

        ClearBackground(BLACK);

        const int texture_x = 0;//SCREEN_WIDTH / 2 - texture.width / 2;
        const int texture_y = 0;//SCREEN_HEIGHT / 2 - texture.height / 2;
        DrawTexture(canvas_texture, texture_x, texture_y, WHITE);

        const char* text = "OMG! IT WORKS!";
        const Vector2 text_size = MeasureTextEx(GetFontDefault(), text, 20, 1);
        DrawText(text, SCREEN_WIDTH / 2 - text_size.x / 2, texture_y + texture.height + text_size.y + 10, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
