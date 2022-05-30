import random
import os
from time import sleep

WHITE = '\u001b[37m'
RED = '\u001b[31m'
GREEN = '\u001b[32m'

square = bytes([254]).decode('cp437')
top = bytes([201, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 187]).decode('cp437')
bottom = bytes([200, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 188]).decode('cp437')
n = 0
size = 10

def readNextFrame(size, f):
    frame = list(range(size))
    for i in range(size):
        frame[i] = list(range(size))
        for j in range(size):
            frame[i][j] =  int(f.read(1))

    #if nextMove != "0":
        #pos = frame[size-1].index(2)
        #frame[size-1][pos] = 0

        #if nextMove == "1":
            #pos = min(size-1, pos+1)
        #if nextMove == "2":
            #pos = max(0, pos-1)
    
    #frame[size-1][pos] = 2
    return frame

f = open("output.txt", "r")
#f2 = open("move.txt", "r")

#firstFrame = [[0] * size] * size
#nextFrame = firstFrame

#writeFrameToFile(f, nextFrame, size)

#initialPos = (int)(size/2)

while(n != 10000):
    os.system('clear')
    nextFrame = readNextFrame(size, f)
    print(nextFrame)
    #writeFrameToFile(f, nextFrame, size)
    for i in range(size+2):
        if i == 0:
            #print(WHITE + top, end="")
            print(WHITE + "1")
        elif i == size+1:
            #print(WHITE + bottom, end="")
            print(WHITE + "1")
        #arr = list(10)
        else:
            for j in range(size+2):
                if j == 0 or j == size+1:
                    #print(WHITE + bytes([186]).decode('cp437'), end="")
                    print(WHITE + "1", end="")
                else:
                    #randVal = random.random()
                    #if randVal > 0.9:
                    if nextFrame[i-1][j-1] == 2:
                        #print(GREEN + square, end="")
                        print(GREEN + "8", end="")
                        #arr[j] = 1
                    elif nextFrame[i-1][j-1] == 1:
                        #print(RED + square, end="")
                        print(RED + "0", end="")
                    
                    else:
                        print(" ", end="")
                        #arr[j] = 0
        print("")
            
    print(n)
    sleep(0.1)
    n = n+ 1

f.close()

