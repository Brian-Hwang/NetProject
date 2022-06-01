


# NetProject Team 10 - ✨Dodge!✨
2022 Spring Semester Network Project Team#10


## **About the Project:**

✨Dodge!✨ is a online application project based of NS-3, which simulates the network scenario under various topologies for the Dodge game. 
The Dodge game is a game which the player gets to move right or left, to dodge the bricks falling from the sky.
In our project, our own-made AI tries to send the movement control to the server, to show how the outcome differs depending on the different topologies and network throughput.

## **How to Run the Project:**

**All cammands should run under NetProject/ns-allinone-3.29/ns-3.29/

** Prerequisite:

    ./waf clean
    ./waf configure --build-profile=debug --enable-examples --enable-tests
    ./waf

You can run the base version of ✨Dodge!✨ with the following command:

    ./waf --run scratch/dodge
Once the application is done running, the you can check the game visualization with the following command:

 1. Simple testing visualization (in ~/ns-3.29/scratch folder):

	    python3 display.py

2. Actual Game visualization (WHERE)

	    ... ADD PLEASE!



## **Command Line Variations:**

✨Dodge!✨ can take various command line inputs to show different scenarios.

 1. Topology

		./waf --run "scratch/dodge --protocol=udp"
	    ./waf --run "scratch/dodge --protocol=wifi"

 2. Data Rate

		./waf --run "scratch/dodge --DataRate=500Mbps"

 3. Delay

	 	./waf --run "scratch/dodge --Delay=1ms"

 4. Frame Rate    

		./waf --run "scratch/dodge --FrameRate=0.01"

 5. Speed    

		./waf --run "scratch/dodge --Speed=0.1"

 6. Speed Increment

		./waf --run "scratch/dodge --SpeedInc=0.01"

    
 7. Speed Interval
 
	    ./waf --run "scratch/dodge --SpeedInt=0.1"

## *System configuration**

The System configuration can be shown as below:

<img width = "80%" src="https://github.com/Brian-Hwang/NetProject/config.png"/>
![alt text](https://github.com/Brian-Hwang/NetProject/blob/main/config.png?raw=true "Title")


## **Dodge Game(Actual Game Visualization)**

<img width = "80%" src="https://github.com/Brian-Hwang/NetProject/171389853-246a9840-500e-4179-9ad9-3deb5a671d5e.gif"/>

The Dodge Game takes pro.output_txt file as input, read each line and translate data into player and enemy positions.
Player dodges bricks by moving characters to left or right.
If the player collides with the falling bricks, game stops and prints 'Game Over' screen.
Game measures elapsed time after game starts and prints it out. It helps measuring performance of input data created by own-made AI with network scenarios. 

## **Docker Address**

 A Docker image can be found here: https://hub.docker.com/repository/docker/helmai/team10
 ```
 docker pull helmai/team10:base
 ```
## **Added Files**

/src/applications/model/game-server.cc
/src/applications/model/game-server.h
/src/applications/model/game-client.cc
/src/applications/model/game-client.h
/src/applications/helper/game-server-helper.cc
/src/applications/helper/game-server-helper.h
/src/applications/helper/game-client-helper.cc
/src/applications/helper/game-client-helper.h
/scratch/dodge.cc
/scratch/testManyToOne.cc
/scratch/display.py
(+ game team code)








