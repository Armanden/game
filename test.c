#include "raylib.h"
#include "raymath.h"
#include <stdint.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 750
#define NUM_LEVELS 10
#define MAX_WALLS 30

typedef struct {
    Rectangle rect;
    Color color;
} Wall;

typedef struct {
    Wall walls[MAX_WALLS];
    int wall_count;
    Vector2 player_pos;
    Vector2 goal_pos;
} Level;

typedef struct {
    uint32_t state;
} PRNG;

static uint32_t prng_next(PRNG *rng) {
    uint32_t x = rng->state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng->state = x;
    return x;
}

static float rand_range(PRNG *rng, float min, float max) {
    return min + (float)(prng_next(rng) % 10000) / 10000.0f * (max - min);
}

static void generate_level(Level *level, uint32_t seed, int level_index) {
    PRNG rng;
    rng.state = seed ^ (uint32_t)(level_index * 2654435761u);

    level->wall_count = 10 + (int)(prng_next(&rng) % 10);

    level->player_pos.x = rand_range(&rng, 20, 150);
    level->player_pos.y = rand_range(&rng, SCREEN_HEIGHT - 150, SCREEN_HEIGHT - 20);

    level->goal_pos.x = rand_range(&rng, SCREEN_WIDTH - 200, SCREEN_WIDTH - 40);
    level->goal_pos.y = rand_range(&rng, 20, 150);

    for (int i = 0; i < level->wall_count; i++) {
        float x = rand_range(&rng, 50, SCREEN_WIDTH - 200);
        float y = rand_range(&rng, 50, SCREEN_HEIGHT - 100);
        float w, h;

        if (prng_next(&rng) % 2) {
            w = rand_range(&rng, 80, 250);
            h = rand_range(&rng, 10, 25);
        } else {
            w = rand_range(&rng, 10, 25);
            h = rand_range(&rng, 80, 250);
        }

        level->walls[i].rect = (Rectangle){x, y, w, h};
        level->walls[i].color = DARKGRAY;
    }
}

int main(void) {
    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;

    InitWindow(screenWidth, screenHeight, "Game");
    SetTargetFPS(60);

	//seed planning on creating a menu screen
    uint32_t seed = 246;
    Level levels[NUM_LEVELS];
    for (int i = 0; i < NUM_LEVELS; i++) {
        generate_level(&levels[i], seed, i);
    }

    int current_level = 0;
    float radius = 15;
    float speed = 200;

    float player_x = levels[current_level].player_pos.x;
    float player_y = levels[current_level].player_pos.y;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        Level *level = &levels[current_level];

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
        Vector2 dest_pos = {level->goal_pos.x, level->goal_pos.y};

        bool collision = CheckCollisionCircles(player_pos, radius, dest_pos, radius);

        Color color = GREEN;
        if (collision) {
            color = RED;
            current_level++;
            if (current_level >= NUM_LEVELS) current_level = 0;
            level = &levels[current_level];
            player_x = level->player_pos.x;
            player_y = level->player_pos.y;
        }

        for (int i = 0; i < level->wall_count; i++) {
            if (CheckCollisionCircleRec(player_pos, radius, level->walls[i].rect)) {
                Vector2 closest = {
                    Clamp(player_pos.x, level->walls[i].rect.x, level->walls[i].rect.x + level->walls[i].rect.width),
                    Clamp(player_pos.y, level->walls[i].rect.y, level->walls[i].rect.y + level->walls[i].rect.height)
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

        DrawText(TextFormat("Level: %i / %i  Seed: %u", current_level + 1, NUM_LEVELS, seed), 10, 10, 20, WHITE);

        for (int i = 0; i < level->wall_count; i++) {
            DrawRectangleRec(level->walls[i].rect, level->walls[i].color);
        }

        DrawCircleV(dest_pos, radius + 2, YELLOW);
        DrawCircleV(dest_pos, radius, BLACK);
        DrawCircleV(player_pos, radius, color);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
