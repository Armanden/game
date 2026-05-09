import pygame, sys

mainClock = pygame.time.Clock()
from pygame.locals import*
pygame.init()
pygame.display.set_caption('oansjn')
#screen=pygame.display.set_mode((600,600),0,32)
screen = pygame.display.set_mode((0, 0), pygame.FULLSCREEN)

scroll = [0,0]

slime_idle = pygame.image.load('slime_idle.png').convert_alpha()
w=slime_idle.width
h=slime_idle.height
slime_idle = pygame.transform.scale(slime_idle, (6*w, 6*h))

jump = False
jump_timer = 0
max_jump_time = 10

slime_jumping_1 = pygame.image.load('slime_jump_1.png').convert_alpha()
slime_jumping_1 = pygame.transform.scale(slime_jumping_1, (6*w, 6*h))

slime_jumping_2 = pygame.image.load('slime_jump_2.png').convert_alpha()
slime_jumping_2 = pygame.transform.scale(slime_jumping_2, (6*w, 6*h))

grass_image = pygame.image.load('grassf.png').convert_alpha()
w1=grass_image.width
h1=grass_image.height
grass_image = pygame.transform.scale(grass_image, (6*w1, 6*h1))

dirt_image = pygame.image.load('dirtf.png').convert_alpha()
dirt_image = pygame.transform.scale(dirt_image, (6*w1, 6*h1))

player_image = slime_idle
player_image_finale = slime_idle
player_facing_right=True
player_facing_right_check=True


player = pygame.Rect(100,220,30,30)
player_y_velocity = 0

def load_map(path):
    f = open(path + '.txt','r')
    data = f.read()
    f.close()
    data=data.split('\n')
    game_map =[]
    for row in data:
        game_map.append(list(row))
    return game_map
game_map = load_map('map')


TILE_SIZE=w*6

def collision_test(rect,tiles):
    collisions=[]
    for tile in tiles:
        if rect.colliderect(tile):
            collisions.append(tile)
    return collisions

def move(rect,movement,tiles):
    collision_types={'top':False,'bottom':False,'right':False,'left':False}
    
    rect.x += movement[0]
    collisions=collision_test(rect,tiles)
    for tile in collisions:
        if movement[0] > 0:
            rect.right = tile.left
            collision_types['right']=True
        if movement[0] < 0:
            rect.left = tile.right
            collision_types['left']=True

    rect.y += movement[1]
    collisions=collision_test(rect,tiles)
    for tile in collisions:
        if movement[1] > 0:
            rect.bottom = tile.top
            collision_types['bottom']=True
        if movement[1] < 0:
            rect.top = tile.bottom
            collision_types['top']=True

    return rect,collision_types

right = False
left = False
air_time = 0


while True:
    screen.fill((255,255,255))
    scroll[0] += player.x - scroll[0] - screen.width*2/5
    scroll[1] += player.y - scroll[1] - screen.height/2
#animation
    if player_facing_right != player_facing_right_check or player_image_finale != player_image:
        player_facing_right_check = player_facing_right
        if player_facing_right == True:
            player_image_finale = player_image
        else:
            player_image_finale = pygame.transform.flip(player_image, True, False)
        
    if player_y_velocity == 0:
        player_image = slime_idle
    if player_y_velocity < -5:
        player_image = slime_jumping_1
    elif player_y_velocity < -2:
        player_image = slime_jumping_2
    if player_y_velocity > 2:
        player_image = slime_jumping_2
    

#nesnelerin konumunu güncelleme
    screen.blit(player_image_finale,(player.x-15 - scroll[0],player.y-15 - scroll[1]))
    #pygame.draw.rect(screen,(0,255,0),player)


    tiles=[]
    y=0
    for row in game_map:
        x = 0
        for tile in row:
            if tile == '1':
                screen.blit(dirt_image, (x * TILE_SIZE - scroll[0],y * TILE_SIZE - scroll[1]))
            if tile == '2':
                screen.blit(grass_image, (x * TILE_SIZE - scroll[0], y * TILE_SIZE - scroll[1]))
            if tile != '0':
                tiles.append(pygame.Rect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE))
            x += 1
        y += 1  

    movement= [0,0]
    if right == True:
        movement[0] += 6

    if left == True:
        movement[0] -= 6

    #if you hold space you jump higher
    if jump and jump_timer < max_jump_time:
        player_y_velocity -= 0.55
        jump_timer += 1


    movement[1] += player_y_velocity
    player, collisions = move(player,movement,tiles)

    
    if collisions['bottom']:
        player_y_velocity = 0
        air_time = 0
        jump_timer = 0
    else:
        if air_time < 8:
            air_time += 1
        player_y_velocity+=0.34
        if player_y_velocity>10:
                player_y_velocity=10
    if collisions['top']:
        player_y_velocity = 0

    #for tile in tiles:
        #pygame.draw.rect(screen,(0,0,0),tile)

    for event in pygame.event.get():
        if event.type == QUIT:
            pygame.quit()
            sys.exit()
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_a:
                left=True
                player_facing_right = False
            if event.key == pygame.K_d:
                right=True
                player_facing_right = True
            if air_time < 8:
                if event.key == pygame.K_SPACE:
                    player_y_velocity = -6
                    jump = True


        if event.type == pygame.KEYUP:
            if event.key == pygame.K_a:
                left=False
            if event.key == pygame.K_d:
                right=False
            if event.key == pygame.K_SPACE:
                jump = False

    #fps = str(int(mainClock.get_fps()))
    #print(fps)
    pygame.display.update()
    mainClock.tick(60)