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
#include "ns3/seq-ts-header.h"


uint8_t sub_abs_uint8(uint8_t a, uint8_t b) {
    return a > b ? a - b : b - a;
}

int sub_abs_uint8(uint8_t a, uint8_t b, uint8_t& direction) {
    if (a > b) {
        direction = (uint8_t) 2;
        return static_cast<int>(a - b);
    }
    else if (a < b) {
        direction = (uint8_t) 1;
        return static_cast<int>(b - a);
    }
    else {
        direction = (uint8_t) 0;
        return 0;
    }
}

std::vector<uint8_t> sub_each(std::vector<uint8_t>& v, uint8_t num) {
    std::vector<uint8_t> v1 {};
    for (auto& n : v)
        v1.push_back(sub_abs_uint8(n, num));
    return v1;
}

std::vector<uint8_t> sub_each(std::vector<std::pair<uint8_t, uint8_t>>& v, uint8_t num) {
    std::vector<uint8_t> v1 {};
    for (auto& n : v)
        v1.push_back(sub_abs_uint8(n.first, num));
    return v1;
}

std::vector<uint8_t>::iterator min_element(std::vector<uint8_t>::iterator begin,
                                           std::vector<uint8_t>::iterator end) {
    auto min = begin;
    for (auto it = begin + 1; it != end; ++it)
        if (*min > *it) min = it;
    return min;
}

std::vector<std::pair<uint8_t, uint8_t>>::iterator
min_element(std::vector<std::pair<uint8_t, uint8_t>>::iterator begin,
            std::vector<std::pair<uint8_t, uint8_t>>::iterator end) {
    auto min = begin;
    for (auto it = begin + 1; it != end; ++it) {
        if (min->second > it->second) min = it;
    }
    return min;
}

int8_t moving_algorithm(std::string& input_frame, int current_position)
{

    // move right = 2
    // move left = 1
    // stay = 0
    uint8_t move = 0;
    std::string oneline = "3333333333";
    
    int number_of_obstacles_in_left = 0;
    int number_of_obstacles_in_right = 0;
    int number_of_obstacles_on_current_position = 0;
    //for (int i = 2; i>=0; i--)
    for (int i = 0; i < 3; i++) {
        std::cout << "inputframe:" << input_frame << std::endl;
        for (int j = 0; j < 10; j++) {
            
            oneline[j] = input_frame[i * 10 + j];
            if (input_frame[i * 10 + j] == '1') {
                if (j < 4) {
                    number_of_obstacles_in_left += 1;
                }
                else if (j > 5) {
                    number_of_obstacles_in_right += 1;

                }
                else {
                    number_of_obstacles_on_current_position += 1;
                }

            }

        }
        std::cout << "number of obstacles in left" << number_of_obstacles_in_left << std::endl;
        // if current position is same as the wall.
        // and obstacle is on top of the characters head
        // -> MOVE LEFT OR RIGHT
        // 
        //        100000000
        //        8

        //        100000101
        //                8
        std::cout << "oneline:" << oneline << std::endl;
        std::cout << "oneline[current_position:" << oneline[current_position] << std::endl;
        
        if (i == 2) {
            if (current_position == 9) {
                if (oneline[current_position] == '1') {
                    move = 1;
                    
                    return move;
                }

            }
            else if (current_position == 0) {
                if (oneline[current_position] == '1') {
                    move = 2;
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

        if (i == 2) {
            if (current_position == 9) {
                if (oneline[current_position - 1] == '1') {
                    move = 0;
                    return move;
                }

            }
            else if (current_position == 0) {
                if (oneline[current_position + 1] == '1') {
                    move = 0;
                    return move;
                }
            }
        }
         
        if (i == 2) {

        // two steps further , evade three horizontal blocks 000.
            if (current_position > 0 && current_position < 9) {


                std::string sampleline3 = "3333333333";
                for ( int eds = 0; eds<10; eds++){

                    sampleline3[eds] = input_frame[1*10 + eds];
                    
                }
                if (sampleline3[current_position] == '1'){

                    
                    if (sampleline3[current_position-1] == '1' && sampleline3[current_position+1] == '1'){
                        if (current_position < 8 && current_position > 4){
                            if (oneline[current_position+1] != '1'){
                                move = 2;
                                return move;
                            
                            }
                        
                        }
                        if (current_position > 2 && current_position < 5){
                            if (oneline[current_position-1] != '1'){
                                move = 1;
                                return move;
                            }
                        
                        }
                        
                        
                        

                    
                    }
                }

            }
        }  
         
        if (i == 2) {

        // two steps further 
            if (current_position > 0 && current_position < 9) {


                std::string sampleline2 = "3333333333";
                for ( int eds = 0; eds<10; eds++){

                    sampleline2[eds] = input_frame[1*10 + eds];
                    
                }
                if (sampleline2[current_position] == '1'){

                       
                    if (oneline[current_position-1] == '1' && oneline[current_position+1] == '1'){
                                              
                        if (current_position < 8){
                            if (oneline[current_position+2] != '1'){
                                move = 2;
                                return move;
                            }
                                
                        }
                        if (current_position > 1){
                            if (oneline[current_position-2] != '1'){
                                move = 1;
                                return move;
                            }
                        }
                        
                        

                    
                    }
                }

            }
        }     



        // if two obstacles exist in the side 
        // -> STAY

        //        100010101
        //             8
        if (i == 2) {

            if (current_position > 0 && current_position < 9) {
                if (oneline[current_position - 1] == '1' && oneline[current_position + 1] == '1') {
                    move = 0;
                    return move;

                    std::cout << "\t\t\t\t\t\t" << "sidebyside" << std::endl;
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
        if (i == 2) {
            if (current_position > 0 && current_position < 9) {
                if (oneline[current_position] == '1') {
                    if (oneline[current_position + 1] == '1') {
                        move = 1;
                        

                        return move;
                    }
                    else if (oneline[current_position - 1] == '1') {
                        move = 2;
                        return move;
                    }

                   // if obstacle on top of head...
                    else{
                     if (oneline[current_position+1] != '1'){
                        move = 2;
                        std::cout << "\t\t\t\t\t\t" << "must move right" << std::endl;
                        
                        
                        
                        // if moving right will eventually cause death, move left
                        std::string sampleline = "3333333333";
                        for (int ed = 0; ed<10; ed++){
                            sampleline[ed] = input_frame[1 * 10 + ed];
                        } 
                        if (current_position < 8){
                            if (sampleline[current_position+1] == '1'){
                                move = 1;
                                return move;
                            }
                        }

                        //oneline[j] = input_frame[i * 10 + j];                            
                        return move;
                    }
                     if (oneline[current_position-1] !='1'){
                        std::cout << "\t\t\t\t\t\t" << "must move left" << std::endl;
                        move = 1;
                        return move;
                     }


                    }
                }

            }
        }
       
        /*
        if ( i == 2){


            if (current_position > 2 && current_position < 7 ){
                 std::cout << "\t\t\t\t\t\t" << "freeposition" << std::endl;
                 

                if (number_of_obstacles_in_left < number_of_obstacles_in_right) {

                    move = 2;
        //move = 1
                    return move;
            }
                if (number_of_obstacles_in_left > number_of_obstacles_in_right) {

                    move = 1;
                    return move;
                }
    
    
            }   
    
        } 
        */

    }
    
    std::cout << "\t\t\t\t\t\t" << "noevent" << std::endl;

    /* 
    if ( i == 2){


        if (current_position > 2 && current_position < 7 ){


            if (number_of_obstacles_in_left < number_of_obstacles_in_right) {

                move = 2;
        //move = 1
                return move;
            }
            if (number_of_obstacles_in_left > number_of_obstacles_in_right) {

                move = 1;
                return move;
            }
    
    
        }   
    
    }
   */

    move = 0;
    return move;
    // if not returned until here, error.

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
            m_socket->SetRecvCallback(MakeCallback(&GameUser::SendResponse, this));
        }
        m_currPos = (int)(m_fieldSize/2);
        // indicate that application is running now
        m_running = true;
    }

    void GameUser::SendResponse(Ptr<Socket> socket) {
        NS_LOG_FUNCTION(this);

        Simulator::Cancel(m_endEvent);

        //Receive Packet Frame Here
        Ptr<Packet> packet;
        Address from;
        uint8_t *payload = NULL; 
        while((packet = m_socket->RecvFrom(from))){
            if(packet->GetSize() > 0){
                payload = new uint8_t[packet->GetSize()];
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
        //std::cout << "coke\n";
        std::string input_frame = (char*) payload;
        
        // calculate the desired position to move on
        //std::cout << "algo input frame len: " << input_frame.size() << std::endl;
        int8_t move = moving_algorithm(input_frame, m_currPos);
        

        int sample_move = static_cast<int> (move);
        if (sample_move == 1){
             move = m_currPos - 1;
        }
        else if (sample_move == 2){
            move = m_currPos + 1;    
        }
        else if (sample_move == 0){
            move = m_currPos;
        }


        // number of packet == number of movement 
        // yeah the character will move one tile at a time
        // direction -> left = 1, right = 2
        uint8_t direction = 0;

        

        int num_of_packet = 0;
        NS_LOG_DEBUG("Current Position: " << static_cast<int>(m_currPos) << "\tDesired Position: " << static_cast<int>(move));
        //std::cout << "num of packet to send: " << num_of_packet << std::endl;
        if ((num_of_packet = sub_abs_uint8(move, m_currPos, direction)) > 0) {
            NS_LOG_DEBUG("Moving " << num_of_packet << " times to position " << static_cast<int>(move));
            //std::cout << "dir: " << static_cast<int>(direction) << std::endl;
                //std::cout << "YEAH! SENDING " << static_cast<int>(direction) << std::endl;
            
            char *buf = new char[2];
            buf[0] = direction;
            buf[1] = '\0';
            for (int i = 0; i < num_of_packet; ++i)
                SendPacket(from, buf);
            delete[] buf;
        }

        // User assumes that the character moved exactly what he has ordered
        m_currPos = move;

        if(payload != NULL)
            delete[] payload;

        m_endEvent = Simulator::Schedule(Seconds(2.0), &GameUser::StopApplication, this);
    }

    void GameUser::SendPacket(Address from, char *payload)
    {
        NS_LOG_FUNCTION(this);

        uint8_t *buf = new uint8_t[strlen(payload)];
        for(uint8_t i = 0; i < strlen(payload); i++)
            buf[i] = (uint8_t)payload[i];

        NS_LOG_DEBUG("Converted char to " << buf);
        NS_LOG_DEBUG("Sending " << payload);

        //[TODO] Should this be 2?? Or rather strlen(payload)?
        Ptr<Packet> packet = Create<Packet>(buf, 2);
        NS_LOG_DEBUG("Before Header");
        SeqTsHeader hdr;
        packet->AddHeader(hdr);
        NS_LOG_DEBUG("After Header");
            

        m_txTrace(packet);

        //send packet over socket (this is fully abstracted, no need to change based in TCP/UDP)
        m_socket->SendTo(packet, 0, from);

        //delete[] payload;
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
