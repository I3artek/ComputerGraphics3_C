#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <printf.h>
#include "Canvas.h"
#include "Shape.h"
#include "DrawingFSM.h"

#define SCREEN_WIDTH (CANVAS_SIZE)
#define SCREEN_HEIGHT (CANVAS_SIZE)

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
    // help printed
    printf("L - draw a line\n");
    printf("P - draw a polygon\n");
    printf("C - draw a circle\n");
    printf("E - move a point\n");
    printf("D - delete a shape containing chosen point\n");
    printf("M - move a shape\n");

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    Texture2D texture = LoadTexture(ASSETS_PATH"test.png"); // Check README.md for how this works

    canvas.data = calloc(CANVAS_SIZE * CANVAS_SIZE,sizeof(uint32_t));
    if(canvas.data == nullptr)
    {
        return -1;
    }
    // fill the pixels
    //printf("Fill the pixels - each %d\n", sizeof(pixel));
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

    DrawingFSM *fsm = new DrawingFSM();
    fsm->draw_line();

    while (!WindowShouldClose())
    {
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 pos = GetMousePosition();
            fsm->handle_click(pos);
            //printf("x: %f, y: %f\n", pos.x, pos.y);
        }

        // handle keyboard shortcuts
        if(IsKeyDown(KEY_L)) fsm->draw_line();
        if(IsKeyDown(KEY_P)) fsm->draw_polygon();
        if(IsKeyDown(KEY_C)) fsm->draw_circle();
        if(IsKeyDown(KEY_E)) fsm->edit_shape();
        if(IsKeyDown(KEY_D)) fsm->delete_point();
        if(IsKeyDown(KEY_M)) fsm->move_shape();
        if(IsKeyDown(KEY_B)) fsm->clear();
        if(IsKeyDown(KEY_S)) fsm->save_to_file();
        if(IsKeyDown(KEY_F)) fsm->load_from_file();
        if(IsKeyDown(KEY_A)) fsm->aa_on();
        if(IsKeyDown(KEY_Q)) fsm->aa_off();

        BeginDrawing();

        ClearBackground(BLACK);

        DrawTexture(canvas_texture, 0, 0, WHITE);

//        const char* text = "OMG! IT WORKS!";
//        const Vector2 text_size = MeasureTextEx(GetFontDefault(), text, 20, 1);
//        DrawText(text, SCREEN_WIDTH / 2 - text_size.x / 2, texture_y + texture.height + text_size.y + 10, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
