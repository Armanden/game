// a really simple puzzle game
// the next level things can work as the entities changing place or just the environment being different

package main

import rl "vendor:raylib"

Wall :: struct {
    rect: rl.Rectangle,
    color: rl.Color,
}

Level :: struct {
    walls:       #assert([3]Wall) [3]Wall,
    player_pos: rl.Vector2,
    goal_pos:   rl.Vector2,
}

levels := [3]Level{
    {
        walls = {
            { rl.Rectangle{100, 100, 200, 20}, rl.DARKGRAY },
            { rl.Rectangle{300, 200, 20, 150}, rl.DARKGRAY },
            { rl.Rectangle{50, 300, 250, 20}, rl.DARKGRAY },
        },
        player_pos = {20, 20},
        goal_pos = {820, 375},
    },
    {
        walls = {
            { rl.Rectangle{200, 150, 300, 20}, rl.DARKGRAY },
            { rl.Rectangle{500, 100, 20, 200}, rl.DARKGRAY },
            { rl.Rectangle{100, 400, 150, 20}, rl.DARKGRAY },
        },
        player_pos = {30, 50},
        goal_pos = {900, 600},
    },
    {
        walls = {
            { rl.Rectangle{100, 100, 20, 300}, rl.DARKGRAY },
            { rl.Rectangle{300, 200, 300, 20}, rl.DARKGRAY },
            { rl.Rectangle{600, 100, 20, 250}, rl.DARKGRAY },
        },
        player_pos = {50, 700},
        goal_pos = {900, 50},
    },
}

main :: proc() {

    rl.InitWindow(1000, 750, "Game")
    defer rl.CloseWindow()

    rl.SetTargetFPS(60)

    current_level := 0
    radius: f32 = 15
    speed: f32 = 200

    player_x := levels[current_level].player_pos.x
    player_y := levels[current_level].player_pos.y

    for !rl.WindowShouldClose() {

dt := rl.GetFrameTime()

        walls := levels[current_level].walls
        dest_x := levels[current_level].goal_pos.x
        dest_y := levels[current_level].goal_pos.y

        for i in 0..<len(walls) {
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
        dest_pos := rl.Vector2{dest_x, dest_y}

        // goal collision
        collision := rl.CheckCollisionCircles(player_pos, radius, dest_pos, radius)

        color := rl.GREEN
        if collision {
            color = rl.RED
            current_level += 1
            if current_level >= len(levels) {
                current_level = 0
            }
            player_x = levels[current_level].player_pos.x
            player_y = levels[current_level].player_pos.y
        }

        // -------------------------
        // WALL COLLISION
        // -------------------------
        for i in 0..<len(walls) {
            wall := walls[i]
            if rl.CheckCollisionCircleRec(player_pos, radius, wall.rect) {

              //  closest := rl.Vector2{
              //      x = rl.Clamp(player_pos.x, wall.rect.x, wall.rect.x + wall.rect.width),
              //      y =  rl.Clamp(player_pos.y, wall.rect.y, wall.rect.y + wall.rect.height),
              //  }
                closest: rl.Vector2
                closest.x = rl.Clamp(player_pos.x, wall.rect.x, wall.rect.x + wall.rect.width)
                closest.y = rl.Clamp(player_pos.y, wall.rect.y, wall.rect.y + wall.rect.height)

                diff := player_pos - closest
                dist := rl.Vector2Length(diff)

                if dist < radius {
                    normal := rl.Vector2Normalize(diff)

                    if dist == 0 {
                        normal = rl.Vector2{1, 0}
                    }

                    overlap := radius - dist
                    player_pos = player_pos + normal * overlap

                    player_x = player_pos.x
                    player_y = player_pos.y
                }
            }
        }

        // -------------------------
        // DRAWING
        // -------------------------
        rl.BeginDrawing()
        rl.ClearBackground(rl.BLACK)
        
        rl.DrawText(rl.TextFormat("Level: %i", current_level + 1), 10, 10, 20, rl.WHITE)

        for i in 0..<len(walls) {
            wall := walls[i]
            rl.DrawRectangleRec(wall.rect, wall.color)
        }


		rl.DrawCircle(i32(dest_x), i32(dest_y), radius + 2, rl.YELLOW)
		rl.DrawCircle(i32(dest_x), i32(dest_y), radius, rl.BLACK)
		 
        rl.DrawCircle(i32(player_x), i32(player_y), radius, color)

 

        rl.EndDrawing()
    }
}
