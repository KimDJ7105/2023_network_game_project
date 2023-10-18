# 학교 로고 출력 상태
import game_framework
from pico2d import *
import title_state

image = None
logo_time = 0.0

def enter():
    global image
    image = load_image('Sprites/etc/tuk_credit.png')
    pass

def exit():
    global image
    del image
    pass

def update():
    global logo_time
    # global running
    if logo_time > 1.0: # 로고가 뜬지 1초 경과시, 출력 종료
        logo_time = 0
        game_framework.change_state(title_state)
    delay(0.01)
    logo_time += 0.01
    pass

def draw():
   clear_canvas()
   image.draw(500,300)
   update_canvas()
   pass

def handle_events():
    events = get_events()
    # for event in events:
    #     if event.type == SDL_QUIT:
    #         game_framework.quit()
    #     elif event.type == SDL_KEYDOWN and event.key == SDLK_ESCAPE:
    #         game_framework.quit()