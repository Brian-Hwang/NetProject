#include "ns3/address.h"
#include "ns3/address-utils.h"
#include "ns3/log.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/node.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/tp-sender.h"
#include "ns3/string.h"

#include<iostream>
#include<fstream>
#include<string>
#include<stdio.h>

int moving_algorithm(string input_frame, int current_position)  
{

	// move right = 2
	// move left = 1
	// stay = 0
	int move = 0;
	string oneline = "3333333333";

	int number_of_obstacles_in_left = 0;
	int number_of_obstacles_in_right = 0;
	int number_of_obstacles_on_current_position = 0;

	for (int i = 9; i >= 0; i--) {

		for (int j = 0; j < 10; j++) {

			oneline[j] = input_frame[i * 10 + j];
			if (input_frame[i * 10 + j] == '1') {
				if (j < current_position) {
					number_of_obstacles_in_left += 1;
				}
				else if (j > current_position) {
					number_of_obstacles_in_right += 1;

				}
				else {
					number_of_obstacles_on_current_position += 1;
				}

			}

		}

		// if current position is same as the wall.
		// and obstacle is on top of the characters head
		// -> MOVE LEFT OR RIGHT
		// 
		//        100000000
		//        8

		//        100000101
		//                8

		if (i == 9) {
			if (current_position == 9) {
				if (oneline[current_position] == '1') {
					move = 1;
					cout << "moved left. position = wall. obstacle - on top of head";
					cout << endl;
					return move;
				}

			}
			else if (current_position == 0) {
				if (oneline[current_position] == '1') {
					move = 2;
					cout << "moved right. position = wall. obstacle - on top of head";
					cout << endl;
					return move;
				}
			}
		}

		// if current position is same as the wall.
		// and moving results death
		// -> STAY
		// 
		//        011000000
		//        8

		//        100000110
		//                8

		if (i == 9) {
			if (current_position == 9) {
				if (oneline[current_position - 1] == '1') {
					move = 0;
					cout << "stayed . position = wall. obstacle - next to the character";
					cout << endl;
					return move;
				}

			}
			else if (current_position == 0) {
				if (oneline[current_position + 1] == '1') {
					move = 0;
					cout << "stayed . position = wall. obstacle - next to the character";
					cout << endl;
					return move;
				}
			}
		}
		// if two obstacles exist in the side 
		// -> STAY

		//        100010101
		//             8
		if (i == 9) {

			if (current_position > 0 && current_position < 9) {
				if (oneline[current_position - 1] == '1' && oneline[current_position + 1] == '1') {
					move = 0;
					cout << "stayed . position = not wall. two obstacles side by side ";
					cout << endl;
					return move;
				}
			}
		}

		// if obstacle is on top of the characters head
		// if choosing wrong direction may cause death
		// -> MOVE LEFT OR RIGHT

		//        100011001
		//             8

		//        100001101
		//             8
		if (i == 9) {
			if (current_position > 0 && current_position < 9) {
				if (oneline[current_position] == '1') {
					if (oneline[current_position + 1] == '1') {
						move = 2;
						cout << "move right . position = not wall. obstacle left to the character";
						cout << endl;
						return move;
					}
					else if (oneline[current_position - 1] == '1') {
						move = 1;
						cout << "move left . position = not wall. obstacle right to the character";
						cout << endl;
						return move;
					}
				}

			}
		}

		cout << oneline;
		cout << endl;
	}
	cout << "number_of_obstacles_in_left: ";
	cout << number_of_obstacles_in_left;
	cout << endl;
	cout << "number_of_obstacles_in_right: ";
	cout << number_of_obstacles_in_right;
	cout << endl;

	// else, determine direction by counting number of obstacles in each side.

	// 
	// if 	number_of_obstacles_in_left < number_of_obstacles_in_right 
	// move left
	// 
	//        11~~~~~~~~
	//        ~111~~~~~~   when total obstacles in left side is bigger
	//        1000000111
	//           8  


	// if 	number_of_obstacles_in_left > number_of_obstacles_in_right 
	// move right
	// 
	//        00~~~~1~11
	//        ~100~~11~1   when total obstacles in right side is bigger
	//        1000010111
	//           8  
	// 
	// 
	// if 	number_of_obstacles_in_left = number_of_obstacles_in_right 
	// stay
	//        1~~~~~~~~~
	//        ~1~~~~~~~~   when total obstacles in left == total obstacles in right
	//        1000000111
	//             8  
	// 
	if (number_of_obstacles_in_left < number_of_obstacles_in_right) {
		move = 1;
		cout << "move left . position = not wall. more obstacles in right side";
		cout << endl;
		return move;
	}
	if (number_of_obstacles_in_left > number_of_obstacles_in_right) {
		move = 2;
		cout << "move right . position = not wall. more obstacles in left side";
		cout << endl;
		return move;
	}
	if (number_of_obstacles_in_left == number_of_obstacles_in_right) {
		move = 0;
		cout << "stay . position = not wall. same number of obstacles in both sides";
		cout << endl;
		return move;
	}

    // if not returned until here, error.
	return -4;
}


namespace ns3{

NS_LOG_COMPONENT_DEFINE ("TPSender");

NS_OBJECT_ENSURE_REGISTERED (TPSender);

TypeId TPSender::GetTypeId(void){
    static TypeId tid = TypeId ("ns3::TPSender")
        .SetParent<Application>()
        .AddConstructor<TPSender>()
        .AddAttribute("Address", "The address",
                AddressValue(),
                MakeAddressAccessor(&TPSender::m_address),
                MakeAddressChecker())
        .AddAttribute("NPackets", "The total number of packets to send",
                UintegerValue(10000),
                MakeUintegerAccessor(&TPSender::m_nPackets),
                MakeUintegerChecker<uint32_t>())
        .AddAttribute("DataRate", "The data rate",
                DataRateValue(DataRate("500kb/s")),
                MakeDataRateAccessor(&TPSender::m_dataRate),
                MakeDataRateChecker())
        .AddTraceSource("Rx", "A new packet is created and is sent",
                MakeTraceSourceAccessor(&TPSender::m_rxTrace), "ns3::Packet::TracedCallback")
        .AddTraceSource("Tx", "A new packet is created and is sent",
                MakeTraceSourceAccessor(&TPSender::m_txTrace), "ns3::Packet::TracedCallback")
        ;
    return tid;
}

TPSender::TPSender()
    :m_socket(0),
    m_packetSize(1000),
    m_packetsSent(0),
    m_running(false)
{
    NS_LOG_FUNCTION(this);
}

TPSender::~TPSender ()
{
  NS_LOG_FUNCTION (this);
}

void TPSender::StartApplication(void){
    NS_LOG_FUNCTION(this);

    //if there is no socket yet, create one
    if(!m_socket){
        //this is udp for now [TODO] decide UDP or TCP
        TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
        m_socket = Socket::CreateSocket(GetNode(), tid);
        m_socket->Bind();
        m_socket->Connect(m_address);
    }

    //indicate that application is running now
    m_running = true;

}




void TPSender::computeResponse(void){
    NS_LOG_FUNCTION(this);
    
    /*Receive Packet with frame here*/
    string input_frame = "0000000000000010000000000000010000001000010000001010101000000000000000000000100001000000000100000000";
	int current_position = 6;
    /*Insert AI for movement here*/
    uint8_t move = moving_algorithm(input_frame,current_position);


    //assuming that a variable 'move' is set, which is 0 when no movement is required
    if(move){
        char *buf = new char[2];
        buf[0] = (char)move;
        buf[1] = '\0';
        SendPacket(buf);
    }
}

void TPSender::SendPacket(char *payload){
    NS_LOG_FUNCTION(this);

    uint8_t *buf = new uint8_t[strlen(payload)];
    for (uint8_t i = 0; i < strlen(payload); i++)
        buf[i] = (uint8_t)payload[0];
    
    NS_LOG_DEBUG("Converted char to " << buf);

    Ptr<Packet> packet = Create<Packet> (buf, 2);
    
    m_txTrace(packet);

    //send packet over socket (this is fully abstracted, no need to change based on TCP/UDP)
    m_socket->Send(packet);

    m_packetsSent++;

    delete[] payload;
    delete[] buf;
}

void TPSender::StopApplication(){
    NS_LOG_FUNCTION(this);
    m_running = false;
    if(m_sendEvent.IsRunning()){
    
        Simulator::Cancel(m_sendEvent);
    }

    if(m_socket){
        m_socket->Close();
    }
}
    
};
