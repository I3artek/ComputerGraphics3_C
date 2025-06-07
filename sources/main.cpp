#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <printf.h>
#include "Canvas.h"
#include "Shape.h"
#include "DrawingFSM.h"
#include "Bezier.h"

#define RIGHT_BAR_WIDTH (400)
#define RIGHT_BAR (CANVAS_SIZE + 20)

#define SCREEN_WIDTH (CANVAS_SIZE + RIGHT_BAR_WIDTH)
#define SCREEN_HEIGHT (CANVAS_SIZE)

#define WINDOW_TITLE "Window title"

#define BEZIER false

Image canvas = {
        .data = nullptr,
        .width = CANVAS_SIZE,
        .height = CANVAS_SIZE,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
};

Texture2D canvas_texture;
char mouse_pos_text[29];

typedef enum {
    GUI_IDLE = 0,
    GUI_CHOOSE_RED,
    GUI_CHOOSE_GREEN,
    GUI_CHOOSE_BLUE
} gui_state;

int last_key = KEY_NULL;
bool WasKeyDown(KeyboardKey k) {
    return  last_key == k;
}

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

    // do some other GUI preparations
    // https://www.raylib.com/examples.html
    const int color_recs_y = 140;
    Rectangle colorsRecs[4] = {
        {
            .x = CANVAS_SIZE + 20,
            .y = color_recs_y,
            .width = 70,
            .height = 70
        },
        {
            .x = CANVAS_SIZE + 110,
            .y = color_recs_y,
            .width = 70,
            .height = 70
        },
        {
            .x = CANVAS_SIZE + 200,
            .y = color_recs_y,
            .width = 70,
            .height = 70
        },
        {
            .x = CANVAS_SIZE + 290,
            .y = color_recs_y,
            .width = 70,
            .height = 70
        }
    };

    gui_state gs = GUI_IDLE;

    canvas_texture = LoadTextureFromImage(canvas);

    DrawingFSM *fsm = new DrawingFSM();
    //fsm->draw_circle();
    Vector2 mouse_pos = GetMousePosition();

    Vector2 bezier_points[MAX_POINTS];
    int bezier_count;
    int max_bezier = 3;

    char bezier_text[19];
    snprintf(bezier_text, 19, "Bezier points: %d", max_bezier);

    int frame_count = 0;

    while (!WindowShouldClose())
    {
        frame_count++;
        mouse_pos = GetMousePosition();
        int x = (int)mouse_pos.x;
        int y = (int)mouse_pos.y;

        // handle mouse movement
        if(gs == GUI_CHOOSE_RED) {
            fsm->current_color.r = ((float)y / CANVAS_SIZE) * 255;
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) gs = GUI_IDLE;
        } else if(gs == GUI_CHOOSE_GREEN) {
            fsm->current_color.g = ((float)y / CANVAS_SIZE) * 255;
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) gs = GUI_IDLE;
        }else if (gs == GUI_CHOOSE_BLUE) {
            fsm->current_color.b = ((float)y / CANVAS_SIZE) * 255;
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) gs = GUI_IDLE;
        }

        // handle mouse clicks
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (x <= CANVAS_SIZE && y <= CANVAS_SIZE && x > 0 && y > 0) {
                // click inside canvas
                if(BEZIER) {
                    bezier_points[bezier_count++] = mouse_pos;
                    printf("Adding point [%d, %d]\n", x, y);
                    if(bezier_count == max_bezier) {
                        draw_bezier_curve(bezier_points, bezier_count);
                        UpdateTexture(canvas_texture, canvas.data);
                        bezier_count = 0;
                    }
                } else {
                    fsm->handle_click(mouse_pos);
                }
            } else {
                // click on some gui
                if (CheckCollisionPointRec(mouse_pos, colorsRecs[0])) {
                    gs = GUI_CHOOSE_RED;
                } else if (CheckCollisionPointRec(mouse_pos, colorsRecs[1])) {
                    gs = GUI_CHOOSE_GREEN;
                } else if (CheckCollisionPointRec(mouse_pos, colorsRecs[2])) {
                    gs = GUI_CHOOSE_BLUE;
                }
            }
            //printf("x: %f, y: %f\n", pos.x, pos.y);
        }

        //scale
        if(IsKeyDown(KEY_KP_1)) fsm->scale = 1;
        if(IsKeyDown(KEY_KP_2)) fsm->scale = 2;
        // for some reason GetKeyPressed always returns 0
        // so I need to do this manually
        if(IsKeyDown(KEY_L)) last_key = KEY_L;
        if(IsKeyDown(KEY_P)) last_key = KEY_P;
        if(IsKeyDown(KEY_C)) last_key = KEY_C;
        if(IsKeyDown(KEY_E)) last_key = KEY_E;
        if(IsKeyDown(KEY_D)) last_key = KEY_D;
        if(IsKeyDown(KEY_M)) last_key = KEY_M;
        if(IsKeyDown(KEY_B)) last_key = KEY_B;
        if(IsKeyDown(KEY_S)) last_key = KEY_S;
        if(IsKeyDown(KEY_F)) last_key = KEY_F;
        if(IsKeyDown(KEY_A)) last_key = KEY_A;
        if(IsKeyDown(KEY_Q)) last_key = KEY_Q;
        if(IsKeyDown(KEY_K)) last_key = KEY_K;
        if(IsKeyDown(KEY_SLASH)) last_key = KEY_SLASH;
        if(IsKeyDown(KEY_BACKSLASH)) last_key = KEY_BACKSLASH;
        if(IsKeyDown(KEY_EQUAL)) last_key = KEY_EQUAL;
        if(IsKeyDown(KEY_MINUS)) last_key = KEY_MINUS;
        if(IsKeyDown(KEY_LEFT_BRACKET)) last_key = KEY_LEFT_BRACKET;
        if(IsKeyDown(KEY_RIGHT_BRACKET)) last_key = KEY_RIGHT_BRACKET;
        // handle keyboard shortcuts
        // once every second
        if(frame_count >= 60) {
            if(WasKeyDown(KEY_L)) fsm->draw_line();
            if(WasKeyDown(KEY_P)) fsm->draw_polygon();
            if(WasKeyDown(KEY_C)) fsm->draw_circle();
            if(WasKeyDown(KEY_E)) fsm->edit_shape();
            if(WasKeyDown(KEY_D)) fsm->delete_point();
            if(WasKeyDown(KEY_M)) fsm->move_shape();
            if(WasKeyDown(KEY_B)) fsm->clear();
            if(WasKeyDown(KEY_S)) fsm->save_to_file();
            if(WasKeyDown(KEY_F)) fsm->load_from_file();
            if(WasKeyDown(KEY_A)) fsm->aa_on();
            if(WasKeyDown(KEY_Q)) fsm->aa_off();
            if(WasKeyDown(KEY_K)) fsm->clip_poly();
            if(WasKeyDown(KEY_SLASH)) fsm->resize_poly();
            if(WasKeyDown(KEY_BACKSLASH)) fsm->rotate_poly();
            if(WasKeyDown(KEY_EQUAL)) fsm->change_width(true);
            if(WasKeyDown(KEY_MINUS)) fsm->change_width(false);
            if(WasKeyDown(KEY_RIGHT_BRACKET) && max_bezier < MAX_POINTS) {
                max_bezier++;
                snprintf(bezier_text, 19, "Bezier points: %d", max_bezier);
            }
            if(WasKeyDown(KEY_LEFT_BRACKET) && max_bezier > 1) {
                max_bezier--;
                snprintf(bezier_text, 19, "Bezier points: %d", max_bezier);
            }
            frame_count = 0;
            last_key = KEY_NULL;
        }

        BeginDrawing();

        ClearBackground(BLACK);
        // draw the canvas
        DrawTexture(canvas_texture, 0, 0, WHITE);
        // draw the gui
        DrawText(states[fsm->state], RIGHT_BAR, 20, 20, GRAY);
        snprintf(mouse_pos_text, 29, "Mouse position: [%d, %d]", (int)mouse_pos.x, (int)mouse_pos.y);
        DrawText(mouse_pos_text, RIGHT_BAR, 50, 20, GRAY);
        DrawText(fsm->width_text, RIGHT_BAR, 80, 20, GRAY);
        DrawText(bezier_text, RIGHT_BAR, 110, 20, GRAY);
        // color selection
        DrawRectangleRec(colorsRecs[0], { fsm->current_color.r, 0, 0, 255});
        DrawRectangleLinesEx(colorsRecs[0], 2, WHITE);
        DrawRectangleRec(colorsRecs[1], { 0, fsm->current_color.g, 0, 255});
        DrawRectangleLinesEx(colorsRecs[1], 2, WHITE);
        DrawRectangleRec(colorsRecs[2], { 0, 0, fsm->current_color.b, 255});
        DrawRectangleLinesEx(colorsRecs[2], 2, WHITE);
        DrawRectangleRec(colorsRecs[3], fsm->current_color);
        DrawRectangleLinesEx(colorsRecs[3], 2, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
