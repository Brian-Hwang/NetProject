from cProfile import run
from platform import platform
from numpy import character
import pygame
import random

pygame.init()
pygame.mixer.init()



screen_width = 480
screen_height = 640
pygame.display.set_caption("Network project") #caption 넣기 
screen = pygame.display.set_mode((screen_width,screen_height))#창크기 지정 

game_font = pygame.font.SysFont("arialrounded", 20)

background = pygame.image.load("background.jpg")
background = pygame.transform.scale(background,(480,640))
#배경 정보

player = pygame.image.load("player_no_background2.png")
player_height = player.get_size()[1]
player_width = player.get_size()[0]
player_pos_x = (screen_width - player_width)/2
player_pos_y = screen_height - player_height
#player 정보

enemy = pygame.image.load("enemy_no_background.png")
enemy_height = enemy.get_size()[1]
enemy_width = enemy.get_size()[0]
enemy_pos_x = random.randint(0, screen_width - player_width + 1)
enemy_pos_y = 0
#enemy 정보

clock = pygame.time.Clock()
running = True
speed = 80

curr_score = 0

def display_score():
    txt_curr_score = game_font.render(f"Score : {curr_score:,}", True, (255,255,255))
    screen.blit(txt_curr_score, (20,20))

def update_score(score):
    global curr_score
    curr_score+=score
#점수 관련

start_ticks = pygame.time.get_ticks()
elapsed_time = 0

def display_time(time):
    txt_curr_time = game_font.render(f"Time : {time}", True, (255,255,255))
    screen.blit(txt_curr_time, (300,20))
#시간 관련

pygame.mixer.music.load( "good.mp3" )
pygame.mixer.music.play()

while running:
    clock.tick(60)
    


    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    key = pygame.key.get_pressed()

    if key[pygame.K_LEFT] ==1:
        player_pos_x -= speed * clock.get_time() / 1000 # 초당으로 하기 때문에 프레임별로 불규칙해질 걱정 없음 

    if key[pygame.K_RIGHT] ==1:
        player_pos_x += speed * clock.get_time() / 1000

    #방향키 눌렀을 때 움직이기 

    if player_pos_x < 0:
        player_pos_x = 0
    
    if player_pos_x > screen_width - player_width:
        player_pos_x = screen_width - player_width

    #화면 밖 나간경우 처리 


    enemy_pos_y += 150 * clock.get_time() / 1000

    if enemy_pos_y > screen_height - enemy_height:
        enemy_pos_x = random.randint(0, screen_width - player_width + 1)
        enemy_pos_y = 0
        update_score(100)

    #적 위치 처리 

    player_rect = player.get_rect()
    player_rect.x = player_pos_x
    player_rect.y = player_pos_y

    enemy_rect = enemy.get_rect()
    enemy_rect.x = enemy_pos_x
    enemy_rect.y = enemy_pos_y

    #충돌 위한 변수

    if player_rect.colliderect(enemy_rect):
        print('충돌')
        running = False
    #충돌 검사 

    screen.blit(background,(0,0))
    screen.blit(player,(player_pos_x,player_pos_y))
    screen.blit(enemy,(enemy_pos_x,enemy_pos_y))
    display_score()
    #점수 표시 추가
    
    elapsed_time = (pygame.time.get_ticks() - start_ticks) / 1000
    display_time(elapsed_time)
    #경과 시간 표시 추가
    pygame.display.update()

pygame.mixer.quit()    
pygame.quit()
