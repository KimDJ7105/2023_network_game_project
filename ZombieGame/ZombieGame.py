import pico2d
import game_framework
import play_state
import title_state
import logo_state

pico2d.open_canvas(1000,600)
game_framework.run(logo_state)
pico2d.close_canvas()
