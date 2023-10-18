from pico2d import *
import play_state
import time
import game_framework
import game_world
import random

PIXEL_PER_METER = (10.0 / 0.3) # 10 pixel 30 cm
RUN_SPEED_KMPH = 10.0 # Km / Hour
RUN_SPEED_MPM = (RUN_SPEED_KMPH * 1000.0 / 60.0)
RUN_SPEED_MPS = (RUN_SPEED_MPM / 60.0)
RUN_SPEED_PPS = (RUN_SPEED_MPS * PIXEL_PER_METER)

class Itembox:
    itembox1_image = None
    itembox2_image = None
    itembox3_image = None
    itembox4_image = None
    itembox5_image = None
    medkit_image = None
    itembox_collision = None
    itembox_spawn = None

    def __init__(self):

        if Itembox.itembox1_image == None:
            Itembox.itembox1_image = load_image('Sprites/ItemBox/itembox01.png')
        if Itembox.itembox2_image == None:
            Itembox.itembox2_image = load_image('Sprites/ItemBox/itembox02.png')
        if Itembox.itembox3_image == None:
            Itembox.itembox3_image = load_image('Sprites/ItemBox/itembox03.png')
        if Itembox.itembox4_image == None:
            Itembox.itembox4_image = load_image('Sprites/ItemBox/itembox04.png')
        if Itembox.itembox5_image == None:
            Itembox.itembox5_image = load_image('Sprites/ItemBox/itembox05.png')

        if Itembox.medkit_image == None:
            Itembox.medkit_image = load_image('Sprites/ItemBox/medkit.png')

        if Itembox.itembox_collision == None:
            Itembox.itembox_collision = load_wav('Sounds/Item/Item_Collision.mp3')
            Itembox.itembox_collision.set_volume(30)

        if Itembox.itembox_spawn == None:
            Itembox.itembox_spawn = load_wav('Sounds/Item/Item_Spawn.mp3')
            Itembox.itembox_spawn.set_volume(20)

        self.speed = 30
        self.x = random.randint(0, 1800)
        self.y = 550
        self.boxmod = random.randint(0,7) # 0 ~ 4 = 박스,  5 ~ 7 : 메디킷
        self.collidable = False
        self.delete = 0


    def update(self):
        self.y -= RUN_SPEED_KMPH * game_framework.frame_time * self.speed

        if self.y <= 60:
            self.y = 60
            self.collidable = True

        if self.delete == 1:
            self.itembox_collision.play()
            game_world.remove_object(self)


    def draw(self):
        sx, sy = self.x - play_state.gamemap.window_left, self.y - play_state.gamemap.window_bottom
        if self.boxmod == 0:
            self.itembox1_image.draw(sx, sy)
        elif self.boxmod == 1:
            self.itembox2_image.draw(sx, sy)
        elif self.boxmod == 2:
            self.itembox3_image.draw(sx, sy)
        elif self.boxmod == 3:
            self.itembox4_image.draw(sx, sy)
        elif self.boxmod == 4:
            self.itembox5_image.draw(sx, sy)
        elif 5 <= self.boxmod <= 7:
            self.medkit_image.draw(sx, sy)
        #draw_rectangle(*self.get_bb())

    def handle_collision(self, player, group):
        if group == 'player:item':
            if self.collidable == True:
                if 5 <= self.boxmod <= 7 and player.hp < 100 and player.transform == 0:
                    #print(self.boxmod, '메디킷 획득')
                    if player.hp <= 50:
                        player.medcheck = 1
                        player.hp += 50
                    elif 100 > player.hp > 50:
                        player.medcheck = 0
                        player.hp = 100
                    player.gettime = get_time()
                    player.boxtype = 5
                    self.delete = 1

                elif 0 <= self.boxmod <= 4 and player.transform == 0:
                    if self.boxmod == 0 or 3 <= self.boxmod <= 4:
                        player.gettime = get_time()
                        player.boxtype = 0
                        play_state.tennis_mag = 30
                        self.delete = 1

                    elif self.boxmod == 1:
                        player.gettime = get_time()
                        player.boxtype = 1
                        play_state.cola_mag = 7
                        self.delete = 1

                    elif self.boxmod == 2:
                        player.gettime = get_time()
                        player.boxtype = 2
                        play_state.bowling_mag = 5
                        self.delete = 1


            else:
                pass

    def get_bb(self):
        return self.x - 25, self.y - 25, self.x + 25, self.y + 25

