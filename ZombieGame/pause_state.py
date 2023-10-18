import game_framework
from pico2d import *
import title_state
import play_state

image = None
logo_time = 0.0

def enter():
    global image
    image = load_image('Sprites/etc/pausescreen.png')
    pass

def exit():
    global image
    del image
    pass

def update():
    pass

def draw():
   clear_canvas()
   play_state.draw_world()
   image.draw(500,300)
   update_canvas()
   pass

def handle_events():
    events = get_events()
    for event in events:
        if event.type == SDL_QUIT:
            game_framework.quit()
        elif event.type == SDL_KEYDOWN:
            match event.key:
                case pico2d.SDLK_ESCAPE:
                    game_framework.change_state(title_state)
                case pico2d.SDLK_p:
                    game_framework.pop_state()