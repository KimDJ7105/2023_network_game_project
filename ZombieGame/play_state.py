from pico2d import *
import game_framework
import pause_state
import game_over_state
import random
from GameMap import Map
import time
from zombieclass import NormalZombie
from zombieclass import FastZombie
from bulletclass import Tennis, Cola
from player_class import Player
from items_class import Itembox
import game_world
import schedule


def handle_events():
    global running, tennis_mag, cola_mag, bulletmod, juggernaut
    events = get_events()
    for event in events:
        if event.type == SDL_QUIT:
            game_framework.quit()
        elif (event.type, event.key) == (SDL_KEYDOWN, SDLK_ESCAPE):
            game_framework.quit()
        elif (event.type, event.key) == (SDL_KEYDOWN, SDLK_p):
            game_framework.push_state(pause_state)
        elif (event.type, event.key) == (SDL_KEYDOWN, SDLK_x):
            if juggernaut == 30:
                juggernaut = 0
                player.transform = 1
                player.transform_time = get_time()
            else:
                pass
        elif (event.type, event.key) == (SDL_KEYDOWN, SDLK_l):
            player.hp = 0

        elif (event.type, event.key) == (SDL_KEYDOWN, SDLK_1):
            player.bulletmod = 0
            print("테니스공")
        elif (event.type, event.key) == (SDL_KEYDOWN, SDLK_2):
            player.bulletmod = 1
            print("콜라")
        elif (event.type, event.key) == (SDL_KEYDOWN, SDLK_3):
            player.bulletmod = 2
            print("볼링공")
        else:
            player.handle_event(event)


player = None
gamemap = None
running = True
n_zombie = None
tennisball = None
cola = None
item = None
tennis_mag = 30
cola_mag = 7
bowling_mag = 5
killcount = 0
juggernaut = 0 # 저거넛 카운트, 30킬마다 해금, 30킬 달성 시 증가하지 않음.
stagelev = 1 # 스테이지 레벨 전역 변수, 3단계까지 존재.
spawnleft = True # True이면 왼쪽, False이면 오른쪽에서 스폰
start = 0
end = 0

def enter():

    global player, gamemap, n_zombie, running, tennis_mag, cola_mag, bowling_mag, killcount, juggernaut, stagelev, spawnleft, start

    start = time.time()
    tennis_mag = 30
    cola_mag = 7
    bowling_mag = 5
    killcount = 0
    juggernaut = 0  # 저거넛 카운트, 30킬마다 해금, 30킬 달성 시 증가하지 않음.
    stagelev = 1  # 스테이지 레벨 전역 변수, 3단계까지 존재.
    spawnleft = True  # True이면 왼쪽, False이면 오른쪽에서 스폰

    gamemap = Map()
    player = Player()

    game_world.add_object(gamemap, 0)
    game_world.add_object(player, 4)

    game_world.add_collision_pairs(player, None, 'player:zombie')
    game_world.add_collision_pairs(player, None, 'player:item')
    running = True

    gamemap.Main_BGM.repeat_play()

# 게임 종료 함수
def exit():
    global end
    end = time.time()
    gamemap.Main_BGM.stop()

    game_world.clear()

def update():
    schedule.run_pending()
    stagelevel()
    for game_object in game_world.all_objects():
        game_object.update()

    if player.hp <= 0:
        game_framework.change_state(game_over_state)

    for a, b, group in game_world.all_collision_pairs():
        if collide(a, b):
            #print('COLLISION ', group)
            a.handle_collision(b, group)
            b.handle_collision(a, group)


    pass



def draw():
    clear_canvas()
    draw_world()
    update_canvas()


def draw_world():
    for game_object in game_world.all_objects():
        game_object.draw()


def pause():
    pass

def resume():
    pass

def collide(a, b):
    left_a, bottom_a, right_a, top_a = a.get_bb()
    left_b, bottom_b, right_b, top_b = b.get_bb()

    if left_a > right_b: return False
    if right_a < left_b: return False
    if top_a < bottom_b: return False
    if bottom_a > top_b: return False

    return True

def enemyspawn():
    global spawnleft
    global stagelev
    if stagelev == 1:
        for i in range(5):
            if spawnleft:
                zombie = NormalZombie(50, 10)
                spawnleft = False
            else :
                zombie = NormalZombie(50, 10, 1950)
                spawnleft = True
            game_world.add_object(zombie, 2)
            game_world.add_collision_pairs(None, zombie, 'player:zombie') # 플레이어 피격, 공격
            game_world.add_collision_pairs(None, zombie, 'zombie:tennis') # 테니스 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:cola') # 콜라병 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:bowling')  # 볼링공 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:bullet')  # 탄환 피격
        zombie.spawn_sound.play()
        for i in range(1):
            if spawnleft:
                zombie = FastZombie(25, 6)
                spawnleft = False
            else:
                zombie = FastZombie(25, 6, 1950)
                spawnleft = True
            game_world.add_object(zombie, 2)
            game_world.add_collision_pairs(None, zombie, 'player:zombie')  # 플레이어 피격, 공격
            game_world.add_collision_pairs(None, zombie, 'zombie:tennis')  # 테니스 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:cola')  # 콜라병 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:bowling')  # 볼링공 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:bullet')  # 탄환 피격
    elif stagelev == 2:
        for i in range(6):
            if spawnleft:
                zombie = NormalZombie(50, 15)
                spawnleft = False
            else:
                zombie = NormalZombie(50, 15, 1950)
                spawnleft = True
            game_world.add_object(zombie, 2)
            game_world.add_collision_pairs(None, zombie, 'player:zombie') # 플레이어 피격, 공격
            game_world.add_collision_pairs(None, zombie, 'zombie:tennis') # 테니스 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:cola') # 콜라병 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:bowling')  # 볼링공 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:bullet')  # 탄환 피격
        zombie.spawn_sound.play()
        for i in range(2):
            if spawnleft:
                zombie = FastZombie(30, 8)
                spawnleft = False
            else:
                zombie = FastZombie(30, 8, 1950)
                spawnleft = True
            game_world.add_object(zombie, 2)
            game_world.add_collision_pairs(None, zombie, 'player:zombie')  # 플레이어 피격, 공격
            game_world.add_collision_pairs(None, zombie, 'zombie:tennis')  # 테니스 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:cola')  # 콜라병 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:bowling')  # 볼링공 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:bullet')  # 탄환 피격

    elif stagelev == 3:
        for i in range(7):
            if spawnleft:
                zombie = NormalZombie(70, 20)
                spawnleft = False
            else:
                zombie = NormalZombie(70, 20, 1950)
                spawnleft = True
            game_world.add_object(zombie, 2)
            game_world.add_collision_pairs(None, zombie, 'player:zombie') # 플레이어 피격, 공격
            game_world.add_collision_pairs(None, zombie, 'zombie:tennis') # 테니스 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:cola') # 콜라병 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:bowling')  # 볼링공 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:bullet')  # 탄환 피격
        zombie.spawn_sound.play()
        for i in range(3):
            if spawnleft:
                zombie = FastZombie(35, 10)
                spawnleft = False
            else:
                zombie = FastZombie(35, 10, 1950)
                spawnleft = True
            game_world.add_object(zombie, 2)
            game_world.add_collision_pairs(None, zombie, 'player:zombie')  # 플레이어 피격, 공격
            game_world.add_collision_pairs(None, zombie, 'zombie:tennis')  # 테니스 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:cola')  # 콜라병 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:bowling')  # 볼링공 피격
            game_world.add_collision_pairs(None, zombie, 'zombie:bullet')  # 탄환 피격

def itemspawn():
    if len(game_world.objects[3]) <= 3: # 최대 4개까지 스폰
        box = Itembox()
        box.itembox_spawn.play(1)
        game_world.add_object(box, 3)
        game_world.add_collision_pairs(None, box, 'player:item')



def stagelevel():
    global stagelev
    if killcount >= 60 and stagelev == 1:
        stagelev = 2
    elif killcount >= 150 and stagelev == 2:
        stagelev = 3



zstart = schedule.every(7).seconds.do(enemyspawn) # 7초마다 5마리씩 스폰
istart = schedule.every(2).seconds.do(itemspawn) # 2초마다 보급 떨어짐, 최대 4개까지만 스폰




