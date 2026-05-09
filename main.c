#include "raylib.h"
#include "raymath.h"
#include <stdint.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 750
#define NUM_LEVELS 10
#define MAX_WALLS 30
#define MAX_ENEMIES 8

typedef struct {
    Rectangle rect;
    Color color;
} Wall;

typedef struct {
    Vector2 pos;
    float radius;
    int hp;
    int max_hp;
    int damage;
    float attack_cooldown;
    float attack_timer;
    bool alive;
} Enemy;

typedef struct {
    Wall walls[MAX_WALLS];
    int wall_count;
    Enemy enemies[MAX_ENEMIES];
    int enemy_count;
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

    level->wall_count = 7 + (int)(prng_next(&rng) % 7);

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

    level->enemy_count = 3 + (int)(prng_next(&rng) % 4);
    for (int i = 0; i < level->enemy_count; i++) {
        level->enemies[i].pos.x = rand_range(&rng, 50, SCREEN_WIDTH - 50);
        level->enemies[i].pos.y = rand_range(&rng, 50, SCREEN_HEIGHT - 50);
        level->enemies[i].radius = 16;
        level->enemies[i].hp = 2;
        level->enemies[i].max_hp = 2;
        level->enemies[i].damage = 1;
        level->enemies[i].attack_cooldown = 1.0f;
        level->enemies[i].attack_timer = 0;
        level->enemies[i].alive = true;
    }
}

typedef enum { MENU, PLAYING } GameState;

int main(void) {
    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;

    InitWindow(screenWidth, screenHeight, "Game");
    SetTargetFPS(60);

    Texture2D slime = LoadTexture("slime_idle.png");
    Texture2D enemy_tex = LoadTexture("enemy.png");

	//seed
    uint32_t seed = 100;
    Level levels[NUM_LEVELS];
    for (int i = 0; i < NUM_LEVELS; i++) {
        generate_level(&levels[i], seed, i);
    }

    int current_level = 0;
    float radius = 15;
    float speed = 200;
    float enemy_speed = 80;
    int player_hp = 5;
    int player_max_hp = 5;
    int player_damage = 1;
    float player_attack_range = 45;
    float player_attack_cooldown = 0.3f;
    float player_attack_timer = 0;

    float player_x = levels[current_level].player_pos.x;
    float player_y = levels[current_level].player_pos.y;

    GameState state = MENU;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (state == MENU) {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                state = PLAYING;
            }

            BeginDrawing();
            ClearBackground(BLACK);

            const char *title = "SLIME GAME";
            int title_font = 80;
            int title_w = MeasureText(title, title_font);
            DrawText(title, (screenWidth - title_w) / 2, screenHeight / 2 - 120, title_font, GREEN);

            DrawCircleV((Vector2){screenWidth / 2 - 120, (float)screenHeight / 2 - 80}, 30, GREEN);
            DrawCircleV((Vector2){screenWidth / 2 + 120, (float)screenHeight / 2 - 80}, 15 + 2, YELLOW);
            DrawCircleV((Vector2){screenWidth / 2 + 120, (float)screenHeight / 2 - 80}, 15, BLACK);

            for (int i = 0; i < 3; i++) {
                float x = screenWidth / 2 - 150 + i * 150;
                DrawRectangle(x, screenHeight / 2 - 30, 20, 120, DARKGRAY);
            }

            const char *prompt = "PRESS ENTER TO PLAY";
            int prompt_font = 30;
            int prompt_w = MeasureText(prompt, prompt_font);
            DrawText(prompt, (screenWidth - prompt_w) / 2, screenHeight / 2 + 100, prompt_font, RAYWHITE);

            const char *credit = "Navigate the labyrinth. Reach the goal. Avoid the walls.";
            int credit_font = 18;
            int credit_w = MeasureText(credit, credit_font);
            DrawText(credit, (screenWidth - credit_w) / 2, screenHeight / 2 + 160, credit_font, GRAY);

            EndDrawing();
            continue;
        }

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

        // wall collision for player
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

        // player attack
        if (player_attack_timer > 0) player_attack_timer -= dt;
        if (IsKeyPressed(KEY_SPACE) && player_attack_timer <= 0) {
            player_attack_timer = player_attack_cooldown;
            for (int i = 0; i < level->enemy_count; i++) {
                if (!level->enemies[i].alive) continue;
                float dist = Vector2Distance(player_pos, level->enemies[i].pos);
                if (dist < player_attack_range) {
                    level->enemies[i].hp -= player_damage;
                    if (level->enemies[i].hp <= 0) {
                        level->enemies[i].alive = false;
                    }
                }
            }
        }

        // enemy movement and attack
        for (int i = 0; i < level->enemy_count; i++) {
            if (!level->enemies[i].alive) continue;

            Enemy *enemy = &level->enemies[i];

            // move toward player
            Vector2 dir = Vector2Subtract(player_pos, enemy->pos);
            float dist = Vector2Length(dir);
            if (dist > 0) {
                Vector2 dir_n = Vector2Normalize(dir);
                Vector2 new_pos = Vector2Add(enemy->pos, Vector2Scale(dir_n, enemy_speed * dt));

                // wall collision for enemy (push-out like player)
                for (int w = 0; w < level->wall_count; w++) {
                    if (CheckCollisionCircleRec(new_pos, enemy->radius, level->walls[w].rect)) {
                        Vector2 closest = {
                            Clamp(new_pos.x, level->walls[w].rect.x, level->walls[w].rect.x + level->walls[w].rect.width),
                            Clamp(new_pos.y, level->walls[w].rect.y, level->walls[w].rect.y + level->walls[w].rect.height)
                        };
                        Vector2 diff = Vector2Subtract(new_pos, closest);
                        float d = Vector2Length(diff);
                        if (d < enemy->radius) {
                            Vector2 n = Vector2Normalize(diff);
                            if (d == 0) n = (Vector2){1, 0};
                            new_pos = Vector2Add(new_pos, Vector2Scale(n, enemy->radius - d));
                        }
                    }
                }
                // enemy-enemy separation
                for (int j = 0; j < level->enemy_count; j++) {
                    if (i == j || !level->enemies[j].alive) continue;
                    Vector2 other_pos = level->enemies[j].pos;
                    Vector2 between = Vector2Subtract(new_pos, other_pos);
                    float between_dist = Vector2Length(between);
                    float min_dist = enemy->radius + level->enemies[j].radius;
                    if (between_dist < min_dist && between_dist > 0) {
                        Vector2 n = Vector2Normalize(between);
                        new_pos = Vector2Add(new_pos, Vector2Scale(n, (min_dist - between_dist) * 0.5f));
                    }
                }
                enemy->pos.x = Clamp(new_pos.x, enemy->radius, screenWidth - enemy->radius);
                enemy->pos.y = Clamp(new_pos.y, enemy->radius, screenHeight - enemy->radius);
            }

            // enemy attack player
            enemy->attack_timer -= dt;
            if (enemy->attack_timer <= 0 && dist < enemy->radius + radius + 5) {
                player_hp -= enemy->damage;
                enemy->attack_timer = enemy->attack_cooldown;
            }
        }

        // check player death
        if (player_hp <= 0) {
            player_hp = player_max_hp;
            player_x = level->player_pos.x;
            player_y = level->player_pos.y;
            player_pos = (Vector2){player_x, player_y};
            for (int i = 0; i < level->enemy_count; i++) {
                level->enemies[i].hp = level->enemies[i].max_hp;
                level->enemies[i].alive = true;
            }
        }

        // goal collision (requires all enemies dead)
        bool all_enemies_dead = true;
        for (int i = 0; i < level->enemy_count; i++) {
            if (level->enemies[i].alive) { all_enemies_dead = false; break; }
        }

        bool goal = all_enemies_dead && CheckCollisionCircles(player_pos, radius, dest_pos, radius);
        if (goal) {
            current_level++;
            if (current_level >= NUM_LEVELS) current_level = 0;
            level = &levels[current_level];
            player_x = level->player_pos.x;
            player_y = level->player_pos.y;
            player_hp = player_max_hp;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawText(TextFormat("Level: %i / %i", current_level + 1, NUM_LEVELS), 10, 10, 20, WHITE);

        // player HP bar
        DrawRectangle(10, 35, 100, 12, GRAY);
        DrawRectangle(10, 35, (int)(100.0f * player_hp / player_max_hp), 12, RED);
        DrawRectangleLines(10, 35, 100, 12, WHITE);

        for (int i = 0; i < level->wall_count; i++) {
            DrawRectangleRec(level->walls[i].rect, level->walls[i].color);
        }

        float enemy_scale = 32.0f / enemy_tex.width;
        for (int i = 0; i < level->enemy_count; i++) {
            if (!level->enemies[i].alive) continue;
            Enemy *enemy = &level->enemies[i];
            DrawTextureEx(enemy_tex, (Vector2){enemy->pos.x - enemy->radius, enemy->pos.y - enemy->radius}, 0, enemy_scale, WHITE);
            // enemy HP bar
            int bar_w = 30;
            DrawRectangle((int)enemy->pos.x - bar_w/2, (int)enemy->pos.y - enemy->radius - 8, bar_w, 4, GRAY);
            DrawRectangle((int)enemy->pos.x - bar_w/2, (int)enemy->pos.y - enemy->radius - 8, (int)(bar_w * (float)enemy->hp / enemy->max_hp), 4, RED);
        }

        DrawCircleV(dest_pos, radius + 2, YELLOW);
        DrawCircleV(dest_pos, radius, BLACK);

        if (!all_enemies_dead) {
            const char *msg = "Kill all enemies to unlock the goal!";
            int msg_w = MeasureText(msg, 20);
            DrawText(msg, (screenWidth - msg_w) / 2, 60, 20, YELLOW);
        }
        float scale = radius * 2.0f / slime.width;
        DrawTextureEx(slime, (Vector2){player_pos.x - radius, player_pos.y - radius}, 0, scale, WHITE);

        EndDrawing();
    }

    UnloadTexture(enemy_tex);
    UnloadTexture(slime);
    CloseWindow();
    return 0;
}
