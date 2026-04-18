// a really simple puzzle game

package main

import rl "vendor:raylib"

main :: proc() {
    rl.InitWindow(1000, 750, "Game")
    defer rl.CloseWindow()

    rl.SetTargetFPS(60)

    x: f32 = 4
    y: f32 = 10
    speed: f32 = 200

    for !rl.WindowShouldClose() {
        dt := rl.GetFrameTime()

        if rl.IsKeyDown(rl.KeyboardKey.RIGHT) {
            x += speed * dt
        }
        if rl.IsKeyDown(rl.KeyboardKey.LEFT) {
            x -= speed * dt
        }
        if rl.IsKeyDown(rl.KeyboardKey.UP) {
            y -= speed * dt
        }
        if rl.IsKeyDown(rl.KeyboardKey.DOWN) {
            y += speed * dt
        }

        rl.BeginDrawing()
        rl.ClearBackground(rl.BLACK)

        rl.DrawCircle(i32(x), i32(y), 15,  rl.GREEN)

        rl.EndDrawing()
    }
}
