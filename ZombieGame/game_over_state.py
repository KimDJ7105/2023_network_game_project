import game_framework
from pico2d import *
import title_state
import play_state
import game_world


image = None
timefont = None
gameoverbgm = None

def enter():
    global image, timefont, gameoverbgm
    image = load_image('Sprites/etc/gameover.png')
    gameoverbgm = load_music('Sounds/BGM/GameOver.mp3')
    gameoverbgm.set_volume(25)
    gameoverbgm.play()
    timefont = load_font('Fonts/154_Impact.ttf')
    pass

def exit():
    game_world.clear()
    gameoverbgm.stop()
    global image
    del image

def update():
    pass

def draw():
   clear_canvas()
   image.draw(500,300)
   timefont.draw(400, 300, f'YOU SURVIVED : {int(play_state.end - play_state.start)} SECONDS!', (255, 255, 255))
   update_canvas()
   pass

def handle_events():
    events = get_events()
    for event in events:
        if event.type == SDL_QUIT:
            game_framework.quit()
        elif event.type == SDL_KEYDOWN:
            match event.key:
                case pico2d.SDLK_RETURN:
                    game_framework.change_state(title_state)
                case pico2d.SDLK_ESCAPE:
                    game_framework.quit()