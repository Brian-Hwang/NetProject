import random
import os
from time import sleep
import sys

WHITE = '\u001b[37m'
RED = '\u001b[31m'
GREEN = '\u001b[32m'

square = bytes([254]).decode('cp437')
top = bytes([201, 205, 205, 205, 205, 205, 205, 205,
            205, 205, 205, 187]).decode('cp437')
bottom = bytes([200, 205, 205, 205, 205, 205, 205, 205,
               205, 205, 205, 188]).decode('cp437')
n = 0
size = 10


def readNextFrame(size, f):
    frame = list(range(size))
    for i in range(size):
        frame[i] = list(range(size))
        for j in range(size):
            nextNum = f.read(1)
            if(nextNum == ''):
                return None
            frame[i][j] = int(nextNum)
    return frame


if (len(sys.argv) > 1):
    f = open(sys.argv[1], "r")
else:
    f = open("output.txt", "r")

while(n != 10000):
    nextFrame = readNextFrame(size, f)
    if (nextFrame == None):
        break;
    os.system('clear')
    print(nextFrame)
    for i in range(size+2):
        if i == 0:
            print(WHITE + "1")
        elif i == size+1:
            print(WHITE + "1")
        else:
            for j in range(size+2):
                if j == 0 or j == size+1:
                    print(WHITE + "1", end="")
                else:
                    if nextFrame[i-1][j-1] == 2:
                        print(GREEN + "8", end="")
                    elif nextFrame[i-1][j-1] == 1:
                        print(RED + "0", end="")

                    else:
                        print(" ", end="")
        print("")

    print(n)
    if(len(sys.argv)>2):
        sleep(float(sys.argv[2]))
    else:
        sleep(0.1)
    n = n + 1

print("END OF GAME")
f.close()
