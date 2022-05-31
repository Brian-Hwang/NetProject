
# NetProject Team  - ✨Dodge!✨
2022 Spring Semester Network Project Team#10


**About the Project:**

✨Dodge!✨ is a online application project based of NS-3, which simulates the network scenario under various topologies for the Dodge game. 
The Dodge game is a game which the player gets to move right or left, to dodge the bricks falling from the sky.
In our project, our own-made AI tries to send the movement control to the server, to show how the outcome differs depending on the different topologies and network throughput.

**How to Run the Project:**

**All cammands should run under NetProject/ns-allinone-3.29/ns-3.29/

You can run the base version of ✨Dodge!✨ with the following command:

    ./waf --run scratch/dodge
Once the application is done running, the you can check the game visualization with the following command:

    .py ASDF

**Command Line Variations:**

✨Dodge!✨ can take various command line inputs to show different scenarios.

 1. Topology
	 asdf
 2. User Number
	 asdf
 3. Throughput
	 adsf

**Details of configuration**

The configuration of the project can be split into 3 parts:

 1. Application (Server - Client network)
 2. Game Model
 3. Test Scenario

=====================================================
Added Files:
- src/application/model/tp-sender.cc
- src/application/model/tp-sender.h
- src/application/helper/tp-sender-helper.cc
- src/application/helper/tp-sender-helper.h
- src/application/model/tp-receiver.cc
- src/application/model/tp-receiver.h
- src/application/helper/tp-receiver-helper.cc
- src/application/helper/tp-receiver-helper.h
- scratch/test.cc
  
TODO:
  - Actually transmit the file data from TPSender to TPReceiver
  - Implement some mechanism that stops the sender if it reaches EOF
