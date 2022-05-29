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


uint8_t moving_algorithm(std::string input_frame, int current_position) {
	// move right = 2
	// move left = 1
	// stay = 0
	uint8_t move = 0;
    std::string upper0 {};
    if (input_frame.copy(upper0, 10, 80) != 10) {
        std::cerr << "error at string copy.0" << std::endl;
        exit(1);
    }
    std::string upper2 {};
    if (input_frame.copy(upper1, 10, 70) != 10) {
        std::cerr << "error at string copy.1" << std::endl;
        exit(1);
    }

    /****
     * 0110100101
     * 0001010111
     * ^8    ^
     * the character examines two blocks lying right upper him
     * and find the closest tile that are safe
     */



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
