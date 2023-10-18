from pico2d import *
import play_state
import time
import game_framework
import game_world

PIXEL_PER_METER = (10.0 / 0.3) # 10 pixel 30 cm
RUN_SPEED_KMPH = 10.0 # Km / Hour
RUN_SPEED_MPM = (RUN_SPEED_KMPH * 1000.0 / 60.0)
RUN_SPEED_MPS = (RUN_SPEED_MPM / 60.0)
RUN_SPEED_PPS = (RUN_SPEED_MPS * PIXEL_PER_METER)

class Tennis:
    tennis_image = None
    tennis_hit = None

    def __init__(self, x = 0, y = 0, dir = 1):

        if Tennis.tennis_image == None:
            Tennis.tennis_image = load_image('Sprites/Bullet/tennisball.png')

        if Tennis.tennis_hit == None:
            Tennis.tennis_hit = load_wav('Sounds/Bullet/Tennis_hit.wav')
            Tennis.tennis_hit.set_volume(35)

        self.speed = 50
        self.x = x
        self.y = y
        self.dir = dir
        self.count = 0
        self.delete = 0

    def update(self):
        self.x += self.dir * RUN_SPEED_KMPH * game_framework.frame_time * self.speed
        if self.x <= 20 or self.x >= 2000:
            self.delete = 1

        if self.delete == 1:
            game_world.remove_object(self)

    def draw(self):
        sx, sy = self.x - play_state.gamemap.window_left, self.y - play_state.gamemap.window_bottom
        self.tennis_image.draw(sx, sy)
        #draw_rectangle(*self.get_bb())


    def get_bb(self):
        return self.x - 15, self.y - 15, self.x + 15, self.y + 15

    def handle_collision(self, other, group):
        if group == 'zombie:tennis':
            if play_state.tennis_mag >= 0 and other.dead == 0:
                other.hp -= 20
                other.hit = 1
                other.hit_time = get_time()
                self.tennis_hit.play()
                other.hurt_sound.play()
                self.delete = 1
                print("좀비 체력 : ", other.hp)
                if other.zdir == 1:
                    other.zx -= 100
                elif other.zdir == -1:
                    other.zx += 100








class Cola:
    coke_l_image = None
    coke_r_image = None
    coke_hit = None

    def __init__(self, x = 500, y = 20, dir = 1):
        if Cola.coke_l_image == None:
            Cola.coke_l_image = load_image('Sprites/Bullet/cola_left.png')
        if Cola.coke_r_image == None:
            Cola.coke_r_image = load_image('Sprites/Bullet/cola_right.png')

        if Cola.coke_hit == None:
            Cola.coke_hit = load_wav('Sounds/Bullet/Cola_hit.mp3')
            Cola.coke_hit.set_volume(35)

        self.speed = 30
        self.count = 0
        self.x = x
        self.y = y
        self.dir = dir
        self.delete = 0

    def update(self):
        self.x += self.dir * RUN_SPEED_KMPH * game_framework.frame_time * self.speed
        if self.x <= 20 or self.x >= 2000:
            self.delete = 1

        if self.delete == 1:
            game_world.remove_object(self)

    def draw(self):
        sx, sy = self.x - play_state.gamemap.window_left, self.y - play_state.gamemap.window_bottom
        #draw_rectangle(*self.get_bb())
        if self.dir == -1:
            self.coke_l_image.draw(sx, sy)
        elif self.dir == 1:
            self.coke_r_image.draw(sx, sy)

    def get_bb(self):
        return self.x - 20, self.y - 10, self.x + 45, self.y + 10

    def handle_collision(self, other, group):
        if group == 'zombie:cola':
            if play_state.cola_mag >= 0 and other.dead == 0:
                self.count += 1
                if other.hit == 0:
                    other.hp -= 1
                    other.hit = 1
                    other.hit_time = get_time()
                    self.coke_hit.play()
                    other.hurt_sound.play()
                if other.speed > 1:
                    other.speed /= 2
                print("좀비 체력 : ", other.hp)
                if other.zdir == 1:
                    other.zx -= 5
                elif other.zdir == -1:
                    other.zx += 5

                if self.count == 5:  # 5명 이상의 좀비 타격 시
                    self.delete = 1

class Bowling:
    bowling_image = None
    bowling_hit = None

    def __init__(self, x = 0, y = 0, dir = 1):

        if Bowling.bowling_image == None:
            Bowling.bowling_image = load_image('Sprites/Bullet/bowling_ball.png')

        if Bowling.bowling_hit == None:
            Bowling.bowling_hit = load_wav('Sounds/Bullet/Bowling_hit.mp3')
            Bowling.bowling_hit.set_volume(35)

        self.speed = 80
        self.x = x
        self.y = y
        self.dir = dir
        self.count = 0
        self.delete = 0

    def update(self):
        self.x += self.dir * RUN_SPEED_KMPH * game_framework.frame_time * self.speed
        if self.x <= 20 or self.x >= 2000:
            self.delete = 1

        if self.delete == 1:
            game_world.remove_object(self)

    def draw(self):
        sx, sy = self.x - play_state.gamemap.window_left, self.y - play_state.gamemap.window_bottom
        self.bowling_image.draw(sx, sy)
        #draw_rectangle(*self.get_bb())

    def get_bb(self):
        return self.x - 25, self.y - 25, self.x + 25, self.y + 25

    def handle_collision(self, other, group):
        if group == 'zombie:bowling':
            if play_state.bowling_mag >= 0 and other.dead == 0:
                if other.hit == 0:
                    other.hp -= 65
                    other.hit = 1
                    other.hit_time = get_time()
                    self.bowling_hit.play()
                    other.hurt_sound.play()
                    self.count += 1
                print("좀비 체력 : ", other.hp)
                if other.zdir == 1:
                    other.zx -= 100
                elif other.zdir == -1:
                    other.zx += 100

                if self.count == 10:
                    self.delete = 1

class Bullet:
    bullet_image = None
    bullet_hit = None

    def __init__(self, x = 0, y = 0, dir = 1):

        if Bullet.bullet_image == None:
            Bullet.bullet_image = load_image('Sprites/Bullet/bullet.png')

        if Bullet.bullet_hit == None:
            Bullet.bullet_hit = load_wav('Sounds/Bullet/Bullet_hit.mp3')
            Bullet.bullet_hit.set_volume(40)

        self.speed = 100
        self.x = x
        self.y = y
        self.dir = dir
        self.count = 0
        self.delete = 0

    def update(self):
        self.x += self.dir * RUN_SPEED_KMPH * game_framework.frame_time * self.speed
        if self.x <= 20 or self.x >= 2000:
            self.delete = 1

        if self.delete == 1:
            game_world.remove_object(self)

    def draw(self):
        sx, sy = self.x - play_state.gamemap.window_left, self.y - play_state.gamemap.window_bottom
        self.bullet_image.draw(sx, sy)
        #draw_rectangle(*self.get_bb())
    def get_bb(self):
        return self.x - 30, self.y - 11, self.x + 30, self.y + 11

    def handle_collision(self, other, group):
        if group == 'zombie:bullet':
            if play_state.bowling_mag >= 0 and other.dead == 0:
                if other.hit == 0:
                    other.hp -= 40
                    other.hit = 1
                    other.hit_time = get_time()
                    self.bullet_hit.play()
                    other.hurt_sound.play()
                    self.count += 1
                print("좀비 체력 : ", other.hp)
                if other.zdir == 1:
                    other.zx -= 10
                elif other.zdir == -1:
                    other.zx += 10

                if self.count == 3:
                    self.delete = 1
