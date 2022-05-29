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
#include "ns3/game-user.h"
#include "ns3/string.h"
#include "ns3/vector.h"
#include <cmath>
#include <utility>

/*
int moving_algorithm(std::string input_frame, int current_position)  
{

	// move right = 2
	// move left = 1
	// stay = 0
	int move = 0;
    std::string oneline = "3333333333";

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
                    std::cout << "moved left. position = wall. obstacle - on top of head";
                    std::cout << std::endl;
					return move;
				}

			}
			else if (current_position == 0) {
				if (oneline[current_position] == '1') {
					move = 2;
                    std::cout << "moved right. position = wall. obstacle - on top of head";
                    std::cout << std::endl;
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
                    std::cout << "stayed . position = wall. obstacle - next to the character";
                    std::cout << std::endl;
					return move;
				}

			}
			else if (current_position == 0) {
				if (oneline[current_position + 1] == '1') {
					move = 0;
                    std::cout << "stayed . position = wall. obstacle - next to the character";
                    std::cout << std::endl;
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
                    std::cout << "stayed . position = not wall. two obstacles side by side ";
                    std::cout << std::endl;
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
                        std::cout << "move right . position = not wall. obstacle left to the character";
                        std::cout << std::endl;
						return move;
					}
					else if (oneline[current_position - 1] == '1') {
						move = 1;
                        std::cout << "move left . position = not wall. obstacle right to the character";
                        std::cout << std::endl;
						return move;
					}
				}

			}
		}

        std::cout << oneline;
        std::cout << std::endl;
	}
    std::cout << "number_of_obstacles_in_left: ";
    std::cout << number_of_obstacles_in_left;
    std::cout << std::endl;
    std::cout << "number_of_obstacles_in_right: ";
    std::cout << number_of_obstacles_in_right;
    std::cout << std::endl;

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
        std::cout << "move left . position = not wall. more obstacles in right side";
        std::cout << std::endl;
		return move;
	}
	if (number_of_obstacles_in_left > number_of_obstacles_in_right) {
		move = 2;
        std::cout << "move right . position = not wall. more obstacles in left side";
        std::cout << std::endl;
		return move;
	}
	if (number_of_obstacles_in_left == number_of_obstacles_in_right) {
		move = 0;
        std::cout << "stay . position = not wall. same number of obstacles in both sides";
        std::cout << std::endl;
		return move;
	}

    // if not returned until here, error.
	return -4;
}
*/

uint8_t sub_abs_uint8(uint8_t a, uint8_t b) {
    return a > b ? a - b : b - a;
}

std::vector<uint8_t> sub_each(std::vector<uint8_t>& v, uint8_t num) {
    std::vector<uint8_t> result {};
    for (auto& n : v) 
        result.push_back(sub_abs_uint8(n, num));
    return std::move(result);
}

std::vector<uint8_t> sub_each(std::vector<std::pair<uint8_t, uint8_t>>& v, uint8_t num) {
    std::vector<uint8_t> v1 {};
    for (auto& n : v)
        v1.push_back(sub_abs_uint8(n.first, num));
    return std::move(v1);
}

std::Compare

uint8_t moving_algorithm(std::string input_frame, int current_pos) {
	// move right = 2
	// move left = 1
	// stay = 0
	//uint8_t move = 0;
    char upper0[11] = "";
    if (input_frame.copy(upper0, 10, 80) != 10) {
        std::cerr << "error at string copy.0" << std::endl;
        exit(1);
    }
    else {
        std::cout << "upper0: ";
        std::cout << upper0 << std::endl;
    }
    char upper1[11] = "";
    if (input_frame.copy(upper1, 10, 70) != 10) {
        std::cerr << "error at string copy.1" << std::endl;
        exit(1);
    }
    else {
        std::cout << "upper1: ";
        std::cout << upper1 << std::endl;
    }
    char upper2[11] = "";
    if (input_frame.copy(upper2, 10, 60) != 10) {
        std::cerr << "error at string copy.1" << std::endl;
        exit(1);
    }
    else {
        std::cout << "upper2: ";
        std::cout << upper2 << std::endl;
    }

     /******
     * 1111111011
     * 0111010101
     * 1000101011
     *  aaa a a
     * b   b b b
     *        c
     *     8
     *
     * the logic is fixed and very simple
     * first character should find the "safe" tiles on each three blocks above it.
     * then he calculate the "cost" on each safe tiles right upper him.
     * "cost" can be calculated by the weight * distance from the character at that moment
     * weight is 3 for upper0, 2 for upper1, 1 for upper2
     * for example, we can calculated each "safe" tiles like this
     * from the leftmost "a"
     * a1 = (3 * 3) + (2 * 1) + (1 * 7) = 18
     * a2 = min( (3 * 2) + (2 * 2) + (1 * 3) = 13, (3 * 2) + (2 * 2) + (1 * 7) = 17) = 13
     * a3 = (3 * 1) + (2 * 1) + (1 * 3) = 8
     * a4 = min( (3 * 1) + (2 * 1) + (1 * 3) = 8, (3 * 1) + (2 * 1) + (1 * 1) = 6 ) = 6
     * a5 = (3 * 2) + (2 * 1) + (1 * 1) = 9
     * the it will definitely choose the a4
     * what if there are multiple minimum tiles?
     * then choose the closest tile at upper0 + leftmost
     *******/
    
    std::vector<uint8_t> upper2_safe {};
    for (uint8_t i = 0; i != 10; ++i)
        if (upper2[i] == '0') upper2_safe.push_back(i);


    std::vector<std::pair<uint8_t, uint8_t>> upper0_safe {};
    for (uint8_t i = 0; i != 10; ++i)
        if (upper0[i] == '0') upper0_safe.emplace_back(std::make_pair(i, 3 * sub_abs_uint8(i, current_pos));

    std::vector<std::pair<uint8_t, uint8_t>> upper1_safe {};
    for (uint8_t i = 0; i != 10; ++i) {
        if (upper1[i] == '0') {
            std::vector<uint8_t> temp = sub_each(upper2_safe, i);
            auto min = std::min_element(std::begin(temp), std::end(temp));
            upper1_safe.emplace_back(i, *min);
        }
    }

    std::vector<std::pair<uint8_t, uint8_t>> decision {};
    for (auto& i : upper0_safe) {
        std::vector<uint8_t> temp = sub_each(upper1_safe, i.first);
        auto min = std::min_element(std::begin(temp), std::end(temp));
        decision.emplace_back(i.first,
                              i.second + 2 * (*min) + upper1_safe[std::distance(temp.begin(), min)].second);
    }

    return std::min_element(decision.begin(), decision.end(),
                                      [](const std::pair<uint8_t, uint8_t>& a, const std::pair<uint8_t, uint8_t>& b)
                                      { return a.second < b.second; })->first;
}

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("GameUser");

    NS_OBJECT_ENSURE_REGISTERED(GameUser);

    TypeId GameUser::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::GameUser")
                                .SetParent<Application>()
                                .AddConstructor<GameUser>()
                                .AddAttribute("Address", "The address",
                                              AddressValue(),
                                              MakeAddressAccessor(&GameUser::m_address),
                                              MakeAddressChecker())
                                .AddAttribute("NPackets", "The total number of packets to send",
                                              UintegerValue(10000),
                                              MakeUintegerAccessor(&GameUser::m_nPackets),
                                              MakeUintegerChecker<uint32_t>())
                                .AddAttribute("Port", "Port on which we listen for incoming packets",
                                              UintegerValue(9),
                                              MakeUintegerAccessor(&GameUser::m_port),
                                              MakeUintegerChecker<uint16_t>())
                                .AddAttribute("GameSize", "Size of the game field",
                                              UintegerValue(),
                                              MakeUintegerAccessor(&GameUser::m_fieldSize),
                                              MakeUintegerChecker<uint8_t>())
                                .AddAttribute("DataRate", "The data rate",
                                              DataRateValue(DataRate("500kb/s")),
                                              MakeDataRateAccessor(&GameUser::m_dataRate),
                                              MakeDataRateChecker())
                                .AddTraceSource("Rx", "A new packet is created and is sent",
                                                MakeTraceSourceAccessor(&GameUser::m_rxTrace), "ns3::Packet::TracedCallback")
                                .AddTraceSource("Tx", "A new packet is created and is sent",
                                                MakeTraceSourceAccessor(&GameUser::m_txTrace), "ns3::Packet::TracedCallback");
        return tid;
    }

    GameUser::GameUser()
        : m_currPos(0),
          m_socket(0),
          m_packetSize(1000),
          m_packetsSent(0),
          m_running(false)
    {
        NS_LOG_FUNCTION(this);
    }

    GameUser::~GameUser()
    {
        NS_LOG_FUNCTION(this);
    }

    void GameUser::StartApplication(void)
    {
        NS_LOG_FUNCTION(this);

        // if there is no socket yet, create one
        if (!m_socket)
        {
            // this is udp for now [TODO] decide UDP or TCP
            TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
            m_socket = Socket::CreateSocket(GetNode(), tid);
            if (m_socket->Bind(InetSocketAddress(Ipv4Address::GetAny(), m_port)) == -1)
                NS_FATAL_ERROR("Failed to bind socket");
            //m_socket->Connect(m_address);
            m_socket->SetRecvCallback(MakeCallback(&GameUser::computeResponse, this));
        }
        m_currPos = (int)(m_fieldSize/2);
        // indicate that application is running now
        m_running = true;
    }

    void GameUser::computeResponse(Ptr<Socket> socket){
        NS_LOG_DEBUG("Huh?!?");
        NS_LOG_FUNCTION(this);
        NS_LOG_DEBUG("Huh?!?");

        //Receive Packet Frame Here
        Ptr<Packet> packet;
        Address from;
        NS_LOG_DEBUG("It is the creation of the payload array...");
        uint8_t *payload = NULL; 
        NS_LOG_DEBUG("Created payload array...");
        while((packet = m_socket->RecvFrom(from))){
            NS_LOG_DEBUG("Tried to receive data...");
            if(packet->GetSize() > 0){
                payload = new uint8_t[packet->GetSize()];
                NS_LOG_DEBUG("Successfully Received Data on user");
                //m_totalRx++;
                m_rxTrace(packet);
                packet->CopyData(payload, packet->GetSize());

                NS_LOG_DEBUG("Received " << payload << " from server.");
                if(packet->GetSize() > m_fieldSize * m_fieldSize){
                    NS_LOG_DEBUG("This frame seems too big...");
                }
            } 
        }

        //[TODO] decide whether user keeps its own information about position or if it should extrcat it from frame
        char *input_frame = new char[m_fieldSize * m_fieldSize];

        /*
        for(uint8_t i = 0; i < m_fieldSize * m_fieldSize; i++){
            input_frame[i] = (char)payload[i];
        }
        */
        memcpy(input_frame, payload, m_fieldSize * m_fieldSize);

        //Insert AI Movement here
        uint8_t move = moving_algorithm(input_frame, m_currPos);
        if (move == 1){
            //left
            m_currPos = (m_currPos > 0)? m_currPos - 1
                                       : 0;
        }else if (move == 2){
            //right
            m_currPos = (m_currPos == (m_fieldSize - 1))? m_currPos
                                                        : m_currPos + 1;
        }
        //assuming that a variable 'move' is set, which is 0 when no movement is required
        if(move){
            char *buf = new char[2];
            buf[0] = (char) move;
            buf[1] = '\0';
            SendPacket(from, buf);
        }

        if(payload != NULL)
            delete[] payload;
        delete[] input_frame;
    }

    void GameUser::SendPacket(Address from, char *payload)
    {
        NS_LOG_FUNCTION(this);

        uint8_t *buf = new uint8_t[strlen(payload)];
        for(uint8_t i = 0; i < strlen(payload); i++)
            buf[i] = (uint8_t)payload[i];

        NS_LOG_DEBUG("Converted char to " << buf);

        //[TODO] Should this be 2?? Or rather strlen(payload)?
        Ptr<Packet> packet = Create<Packet>(buf, 2);

        m_txTrace(packet);

        //send packet over socket (this is fully abstracted, no need to change based in TCP/UDP)
        m_socket->SendTo(packet, 0, from);

        delete[] payload;
        delete[] buf;
    }

    void GameUser::StopApplication()
    {
        NS_LOG_FUNCTION(this);
        m_running = false;

        if (m_socket)
        {
            m_socket->Close();
        }
    }

};
