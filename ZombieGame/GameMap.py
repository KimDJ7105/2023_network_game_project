from pico2d import *
import play_state
from zombieclass import NormalZombie
import game_framework
import game_world
from items_class import Itembox

zombie = None
class Map:
    Main_BGM = None
    def __init__(self):
        self.background = load_image('Sprites/Map/background.png')
        self.tile = load_image('Sprites/Map/tile.png')
        self.background_city = load_image('Sprites/Map/city.png')
        self.counter = 0 # 좀비 수 제한 두기
        self.font = load_font('Fonts/154_Impact.ttf')
        self.canvas_width = get_canvas_width()
        self.canvas_height = get_canvas_height()
        self.w = self.background.w
        self.h = self.background.h
        self.window_left = 0
        self.window_bottom = 0

        if Map.Main_BGM == None:
            Map.Main_BGM = load_music('Sounds/BGM/Main_BGM.mp3')
            Map.Main_BGM.set_volume(25)

    def draw(self):
        sx, sy = play_state.player.x - play_state.gamemap.window_left, play_state.player.y - play_state.gamemap.window_bottom
        self.background.clip_draw_to_origin(
            self.window_left,
            self.window_bottom,
            self.canvas_width,
            self.canvas_height, 0, 0)
        self.background_city.clip_draw_to_origin(
            self.window_left,
            self.window_bottom,
            self.canvas_width,
            500, 0, -100)
        self.tile.clip_draw_to_origin(
            self.window_left,
            self.window_bottom,
            self.canvas_width,
            self.canvas_height, 0, -50)

        # UI 관련 코드들
        self.font.draw(10, 550, f'(HP: {play_state.player.hp})', (255, 255, 255))  # 플레이어 HP 출력
        self.font.draw(10, 500,  f'(KILLCOUNT: {play_state.killcount})', (255, 255, 255)) # 킬 점수 출력

        self.font.draw(835, 575, '[JUGGERNAUT MOD]', (255, 255, 255))
        self.font.draw(835, 550, '[', (255, 255, 255))
        self.font.draw(990, 550, ']', (255, 255, 255))
        for i in range(play_state.juggernaut):
            self.font.draw(840 + (i*5), 550, 'I', (0, 255, 0))  # 저거넛 스택

        if play_state.juggernaut == 30:
            self.font.draw(10, 475, f'[!JUGGERNAUT READY!]', (255, 255, 0))  # 킬 점수 출력

        if play_state.player.transform == 0:
            if play_state.player.bulletmod == 0: # 탄환 종류, 남은 잔여 탄환 개수 출력
                if play_state.tennis_mag > 5:
                    self.font.draw(10, 525, f'(TENNISBALL | {play_state.tennis_mag}/30)', (255, 255, 255))  # 테니스볼 개수 출력
                elif play_state.tennis_mag <= 5:
                    self.font.draw(10, 525, f'(TENNISBALL | {play_state.tennis_mag}/30)', (255, 0, 0))  # 테니스볼 개수 출력
                    self.font.draw(sx - 100, sy + 100, 'WARNING! LOW AMMUNITION!', (255, 0, 0)) # 탄 부족 메시지
            elif play_state.player.bulletmod == 1:
                if play_state.cola_mag > 2:
                    self.font.draw(10, 525, f'(JOKA-COLA | {play_state.cola_mag}/7)', (255, 255, 255))  # 콜라병 개수 출력
                elif play_state.cola_mag <= 2:
                    self.font.draw(10, 525, f'(JOKA-COLA | {play_state.cola_mag}/7)', (255, 0, 0))  # 콜라병 개수 출력
                    self.font.draw(sx - 100, sy + 100, 'WARNING! LOW AMMUNITION!', (255, 0, 0)) # 탄 부족 메시지
            elif play_state.player.bulletmod == 2:
                if play_state.bowling_mag > 2:
                    self.font.draw(10, 525, f'(BOWLINGBALL | {play_state.bowling_mag}/5)', (255, 255, 255))  # 볼링공 개수 출력
                elif play_state.bowling_mag <= 2:
                    self.font.draw(10, 525, f'(BOWLINGBALL | {play_state.bowling_mag}/5)', (255, 0, 0))  # 볼링공 개수 출력
                    self.font.draw(sx - 100, sy + 100, 'WARNING! LOW AMMUNITION!', (255, 0, 0)) # 탄 부족 메시지
        elif play_state.player.transform == 1:
            self.font.draw(10, 525, f'(6.8mm XM1186 | INFINITE)', (255, 255, 0))  # 볼링공 개수 출력




    def update(self):
        self.window_left = clamp(0, int(play_state.player.x) - self.canvas_width // 2,
                                 self.w - self.canvas_width - 1)
        self.window_bottom = clamp(0, int(play_state.player.y) - self.canvas_height // 2,
                                   self.h - self.canvas_height - 1)









