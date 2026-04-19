// a really simple puzzle game

package main

import rl "vendor:raylib"

main :: proc() {
    rl.InitWindow(1000, 750, "Game")
    defer rl.CloseWindow()

    rl.SetTargetFPS(60)

    player_x: f32 = 20
    player_y: f32 = 20
    speed: f32 = 200
    radius: f32 = 15
    dest_x: f32 = 820
    dest_y: f32 = 375

    for !rl.WindowShouldClose() {
        dt := rl.GetFrameTime()

// movment and borders
        if (rl.IsKeyDown(rl.KeyboardKey.RIGHT) && (player_x + radius + speed * dt <= 1000  )) {
            player_x += speed * dt
        }
        if (rl.IsKeyDown(rl.KeyboardKey.LEFT) && (player_x - radius - speed * dt >= 0) ){
            player_x -= speed * dt
        }
        if (rl.IsKeyDown(rl.KeyboardKey.UP) && (player_y - radius - speed *dt >= 0) ){
            player_y -= speed * dt
        }
        if (rl.IsKeyDown(rl.KeyboardKey.DOWN) && (player_y + radius + speed * dt <= 750) ){
            player_y += speed * dt
        }
       
		player_pos := rl.Vector2{player_x, player_y}
		dest_pos   := rl.Vector2{dest_x, dest_y}

		collision := rl.CheckCollisionCircles(player_pos, radius, dest_pos, radius)

		color := rl.GREEN
		if (collision) {
		    color = rl.RED
		    //there should be the function where it skips to the next level
		    // f_nextlevel() current++ so basically just an adder
		}


		rl.BeginDrawing()
		rl.DrawCircle(i32(player_x), i32(player_y), radius, color)
		rl.DrawCircle(i32(dest_x), i32(dest_y), radius + 2, rl.YELLOW) //the ring
     	rl.DrawCircle(i32(dest_x), i32(dest_y), radius, rl.BLACK)	
        rl.ClearBackground(rl.BLACK)

        rl.EndDrawing()
    }
}
