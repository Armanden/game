#include "raylib.h"
#include "raymath.h"

typedef struct {
    Rectangle rect;
    Color color;
} Wall;

typedef struct {
    Wall walls[3];
    Vector2 player_pos;
    Vector2 goal_pos;
} Level;

static const Level levels[3] = {
    {
        .walls = {
            { {100, 100, 200, 20}, DARKGRAY },
            { {300, 200, 20, 150}, DARKGRAY },
            { {50, 300, 250, 20}, DARKGRAY },
        },
        .player_pos = {20, 20},
        .goal_pos = {820, 375},
    },
    {
        .walls = {
            { {200, 150, 300, 20}, DARKGRAY },
            { {500, 100, 20, 200}, DARKGRAY },
            { {100, 400, 150, 20}, DARKGRAY },
        },
        .player_pos = {30, 50},
        .goal_pos = {900, 600},
    },
    {
        .walls = {
            { {100, 100, 20, 300}, DARKGRAY },
            { {300, 200, 300, 20}, DARKGRAY },
            { {600, 100, 20, 250}, DARKGRAY },
        },
        .player_pos = {50, 700},
        .goal_pos = {900, 50},
    },
};

int main(void) {
    const int screenWidth = 1000;
    const int screenHeight = 750;

    InitWindow(screenWidth, screenHeight, "Game");
    SetTargetFPS(60);

    int current_level = 0;
    float radius = 15;
    float speed = 200;

    float player_x = levels[current_level].player_pos.x;
    float player_y = levels[current_level].player_pos.y;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        const Wall *walls = levels[current_level].walls;
        float dest_x = levels[current_level].goal_pos.x;
        float dest_y = levels[current_level].goal_pos.y;

        if (IsKeyDown(KEY_RIGHT) && player_x + radius + speed * dt <= screenWidth) {
            player_x += speed * dt;
        }
        if (IsKeyDown(KEY_LEFT) && player_x - radius - speed * dt >= 0) {
            player_x -= speed * dt;
        }
        if (IsKeyDown(KEY_UP) && player_y - radius - speed * dt >= 0) {
            player_y -= speed * dt;
        }
        if (IsKeyDown(KEY_DOWN) && player_y + radius + speed * dt <= screenHeight) {
            player_y += speed * dt;
        }

        Vector2 player_pos = {player_x, player_y};
        Vector2 dest_pos = {dest_x, dest_y};

        bool collision = CheckCollisionCircles(player_pos, radius, dest_pos, radius);

        Color color = GREEN;
        if (collision) {
            color = RED;
            current_level++;
            if (current_level >= 3) current_level = 0;
            player_x = levels[current_level].player_pos.x;
            player_y = levels[current_level].player_pos.y;
        }

        for (int i = 0; i < 3; i++) {
            if (CheckCollisionCircleRec(player_pos, radius, walls[i].rect)) {
                Vector2 closest = {
                    Clamp(player_pos.x, walls[i].rect.x, walls[i].rect.x + walls[i].rect.width),
                    Clamp(player_pos.y, walls[i].rect.y, walls[i].rect.y + walls[i].rect.height)
                };

                Vector2 diff = {player_pos.x - closest.x, player_pos.y - closest.y};
                float dist = Vector2Length(diff);

                if (dist < radius) {
                    Vector2 normal = Vector2Normalize(diff);
                    if (dist == 0) normal = (Vector2){1, 0};

                    float overlap = radius - dist;
                    player_pos = Vector2Add(player_pos, Vector2Scale(normal, overlap));

                    player_x = player_pos.x;
                    player_y = player_pos.y;
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawText(TextFormat("Level: %i", current_level + 1), 10, 10, 20, WHITE);

        for (int i = 0; i < 3; i++) {
            DrawRectangleRec(walls[i].rect, walls[i].color);
        }

        DrawCircleV(dest_pos, radius + 2, YELLOW);
        DrawCircleV(dest_pos, radius, BLACK);
        DrawCircleV(player_pos, radius, color);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}