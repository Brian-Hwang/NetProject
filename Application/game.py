from time import sleep
from cProfile import run
from platform import platform
from numpy import character
import pygame
import random

pygame.init()
pygame.mixer.init()

gameplay_font = pygame.font.SysFont("dejavuserif", 20)
endgame_font =  pygame.font.SysFont("dejavuserif", 50)

fd = open('pro_output.txt', 'r')
count = 0
while True:
    data = fd.readline()
    if not data: 
        break
    if data.count('1') > count:
        count = data.count('1')
count += 9
fd.close()

on = 0
screen_width = 500
screen_height = 500
pygame.display.set_caption("Network project")  # caption 넣기
screen = pygame.display.set_mode((screen_width, screen_height))  # 창크기 지정


background = pygame.image.load("background.jpg")
background = pygame.transform.scale(background, (500, 500))
#배경 정보

player = pygame.image.load("player_no_background2.png")
player = pygame.transform.scale(player, (50, 50))
player_height = player.get_size()[1]
player_width = player.get_size()[0]
player_pos_x = 5 * 50
player_pos_y = screen_height - player_height
#player 정보

enemy = []
enemy_height = []
enemy_width = []
enemy_pos_x = []
enemy_pos_y = []
for i in range(count):
    enemy.append(pygame.image.load("enemy_no_background.png"))
    enemy[i] = pygame.transform.scale(enemy[i], (50, 50))
    enemy_height.append(enemy[i].get_size()[1])
    enemy_width.append(enemy[i].get_size()[0])
    enemy_pos_x.append(-50)
    enemy_pos_y.append(-50)
#enemy 정보
which_enemy = 0

clock = pygame.time.Clock()
running = 1
speed = 80

start_ticks = pygame.time.get_ticks()
elapsed_time = 0
end_time =0

def display_time(time):
    curr_time = gameplay_font.render(f"Time : {time}", True, (255, 255, 255))
    screen.blit(curr_time, (370, 20))
#시간 표시

def end_game(time):
    end_time = endgame_font.render(f"Final Time : {time}", True, (255, 255, 255))
    game_over = endgame_font.render(f"Game Over", True, (255, 255, 255))
    screen.blit(game_over, (130, 150))
    screen.blit(end_time, (70, 250))

pygame.mixer.music.load("song.mp3")
pygame.mixer.music.play()


case1=[]
case2=[]
case3=[]
len_one = 0 
one_index = []
here = ""
fd = open('pro_output.txt', 'r')
sleep(2)
while (running ==1 or running ==2):
    sleep(0.1)
    on +=1
    predata = data
    data = fd.readline()
    here = data[90:100]

    if not data:
        break
    
    case2 =[]
    if len(case1)!=0:
        for i in range(len(case1)):
            val = case1[i]+10
            case2.append(val)
    else:
        case2 =[]
    one_index = []
    two_index = 5
    case1 =[]
    case3 =[]

    for i in range(100):
        if data[i] == '1':
            one_index.append(i)
            if i // 10 ==8:
                case1.append(i)
            if i // 10 ==9:
                case3.append(i) 
        if data[i] == '2':
            two_index = i

    player_pos_x = (two_index % 10) * 50
    len_one = len(one_index)
    
    
    #플레이어 초기 위치

    if len_one !=0:
        for i in range(len_one):
            enemy_pos_x[i] = (one_index[i] % 10) * 50
            enemy_pos_y[i] = (one_index[i] // 10) * 50
        
        
        for i in range(len_one,count):
            enemy_pos_x[i] = -50
            enemy_pos_y[i] = -50

    clock.tick(60)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = 0

    
    if player_pos_x < 0:
        player_pos_x = 50 * two_index
         
    if player_pos_x > screen_width - player_width:
         player_pos_x = screen_width - player_width

    #화면 밖 나간경우 처리 


    #적 위치 처리 

    player_rect = player.get_rect()
    player_rect.x = player_pos_x
    player_rect.y = player_pos_y
    

    enemy_rect = []
    for i in range(count):
        enemy_rect.append(enemy[i].get_rect())
        enemy_rect[i].x=enemy_pos_x[i]
        enemy_rect[i].y = enemy_pos_y[i]

    for i in range(count):
        if player_rect.colliderect(enemy_rect[i]):
            end_time = elapsed_time
            running = 2
    #충돌 검사 


    
    one_index = []
    two_index = 5

    if(running==1):
        screen.blit(background,(0,0))
        screen.blit(player,(player_pos_x,player_pos_y))
        for i in range(count):
            screen.blit(enemy[i],(enemy_pos_x[i],enemy_pos_y[i]))
    
        elapsed_time = (pygame.time.get_ticks() - start_ticks) / 1000
        display_time(elapsed_time)
    #경과 시간 표시 추가
        
    if(elapsed_time > 5000 and running == 1):
        end_time = elapsed_time
        running = 2 #end screen 출력
    
    if(running == 2):
        end_game(end_time)

    if len(case2)!=0 and case2 != case3 and predata[90:100].count('1')!=data[90:100].count('1'):
        print(predata)
        print(data)
        end_time = elapsed_time
        running =2
        
    pygame.display.update()   

    if(running ==2):
        sleep(2)
        running = 0
        
    
    #게임 종료 시 최종 시간 출력

print(on)    
fd.close()

pygame.mixer.quit()    
pygame.quit()
